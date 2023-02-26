#include"BchEncoder.h"



void BchEncoder::get_px(int m)
/*
*  Read m, the degree of a primitive polynomial p(x) used to compute the
*  Galois field GF(2**m). Get precomputed coefficients p[] of p(x). Read
*  the code length.
*/
{
	p[0] = p[m] = 1;
	for (int i = 1; i<m; i++)
		p[i] = 0;

	if (m == 3) p[1] = 1;
	else if (m == 4)    p[1] = 1;
	else if (m == 5)    p[2] = 1;
	else if (m == 6)    p[1] = 1;
	else if (m == 7)    p[3] = 1;
	else if (m == 8)    p[2] = p[3] = p[4] = 1;
	else if (m == 9)    p[4] = 1;
	else if (m == 10)   p[3] = 1;
	else if (m == 11)   p[2] = 1;
	else if (m == 12)   p[3] = p[4] = p[7] = 1;
	else if (m == 13)   p[1] = p[3] = p[4] = 1;
	else if (m == 14)   p[1] = p[11] = p[12] = 1;
	else if (m == 15)   p[1] = 1;
	else if (m == 16)   p[2] = p[3] = p[5] = 1;
	else if (m == 17)   p[3] = 1;
	else if (m == 18)   p[7] = 1;
	else if (m == 19)   p[1] = p[5] = p[6] = 1;
	else if (m == 20)   p[3] = 1;

}

void BchEncoder::generate_gf(int n, int m)
/*
* Generate field GF(2**m) from the irreducible polynomial p(X) with
* coefficients in p[0]..p[m].
*
* Lookup tables:
*   index->polynomial form: alpha_to[] contains j=alpha^i;
*   polynomial form -> index form:  index_of[j=alpha^i] = i
*
* alpha=2 is the primitive element of GF(2**m)
*/
{
	int mask;
	mask = 1;
	alpha_to[m] = 0;
	for (int i = 0; i < m; i++) {
		alpha_to[i] = mask;
		index_of[alpha_to[i]] = i;
		if (p[i] != 0)
			alpha_to[m] ^= mask;//Òì»ò
		mask <<= 1;//  2^(m-1)
	}
	index_of[alpha_to[m]] = m;
	mask >>= 1;//2^m
	for (int i = m + 1; i < n; i++) {
		if (alpha_to[i - 1] >= mask)
			alpha_to[i] = alpha_to[m] ^ ((alpha_to[i - 1] ^ mask) << 1);
		else
			alpha_to[i] = alpha_to[i - 1] << 1;
		index_of[alpha_to[i]] = i;
	}
	index_of[0] = -1;
}

int BchEncoder::generate_gx(int n, int m, int t)
/*
* Compute the generator polynomial of a binary BCH code. Fist generate the
* cycle sets modulo 2**m - 1, cycle[][] =  (i, 2*i, 4*i, ..., 2^l*i). Then
* deterMine those cycle sets that contain integers in the set of (d-1)
* consecutive integers {1..(d-1)}. The generator polynomial is calculated
* as the product of linear factors of the form (x+alpha^i), for every i in
* the above cycle sets.
*/
{
	int    ii, jj, ll, kaux;
	int    test, aux, nocycles, root, noterms, rdncy;
	int    cycle[1024][21], size[1024], Min[1024], zeros[1024];

	/* Generate cycle sets modulo n, n = 2**m - 1 */
	cycle[0][0] = 0;
	size[0] = 1;
	cycle[1][0] = 1;
	size[1] = 1;
	jj = 1;         /* cycle set index */
	if (m > 9)  {
		printf("Computing cycle sets modulo %d\n", n);
		printf("(This may take some time)...\n");
	}
	do {
		/* Generate the jj-th cycle set */
		ii = 0;
		do {
			ii++;
			cycle[jj][ii] = (cycle[jj][ii - 1] * 2) % n;
			size[jj]++;
			aux = (cycle[jj][ii] * 2) % n;
		} while (aux != cycle[jj][0]);
		/* Next cycle set representative */
		ll = 0;
		do {
			ll++;
			test = 0;
			for (ii = 1; ((ii <= jj) && (!test)); ii++)
				/* ExaMine previous cycle sets */
				for (kaux = 0; ((kaux < size[ii]) && (!test)); kaux++)
					if (ll == cycle[ii][kaux])
						test = 1;
		} while ((test) && (ll < (n - 1)));
		if (!(test)) {
			jj++;   /* next cycle set index */
			cycle[jj][0] = ll;
			size[jj] = 1;
		}
	} while (ll < (n - 1));
	nocycles = jj;      /* number of cycle sets modulo n */

	int d = 2 * t + 1;

	/* Search for roots 1, 2, ..., d-1 in cycle sets */
	kaux = 0;
	rdncy = 0;
	for (ii = 1; ii <= nocycles; ii++) {
		Min[kaux] = 0;
		test = 0;
		for (jj = 0; ((jj < size[ii]) && (!test)); jj++)
			for (root = 1; ((root < d) && (!test)); root++)
				if (root == cycle[ii][jj])  {
					test = 1;
					Min[kaux] = ii;
				}
		if (Min[kaux]) {
			rdncy += size[Min[kaux]];
			kaux++;
		}
	}
	noterms = kaux;
	kaux = 1;
	for (ii = 0; ii < noterms; ii++)
		for (jj = 0; jj < size[Min[ii]]; jj++) {
			zeros[kaux] = cycle[Min[ii]][jj];
			kaux++;
		}

	int k = n - rdncy;

	if (k<0)
	{
		printf("Parameters invalid!\n");
		exit(0);
	}

	printf("This message using  (%d, %d, %d) binary BCH code\n", n, k, d);

	/* Compute the generator polynomial */
	g[0] = alpha_to[zeros[1]];
	g[1] = 1;       /* g(x) = (X + zeros[1]) initially */
	for (ii = 2; ii <= rdncy; ii++) {
		g[ii] = 1;
		for (jj = ii - 1; jj > 0; jj--)
			if (g[jj] != 0)
				g[jj] = g[jj - 1] ^ alpha_to[(index_of[g[jj]] + zeros[ii]) % n];
			else
				g[jj] = g[jj - 1];
		//     printf("g[jj] = %d\n",g[jj]);   
		g[0] = alpha_to[(index_of[g[0]] + zeros[ii]) % n];
	}
	printf("Generator polynomial:   g(x) = ");
	for (ii = 0; ii <= rdncy; ii++) {
		printf("%d", g[ii]);
		if (ii && ((ii % 50) == 0))
			printf("\n");
	}
	printf("\n");
	return k;
}


Vector<int> BchEncoder::add_0bits(Vector<int>a, int& blocknum, int k)
{
	Vector<int>b;
	int codelength = a.size();
	if ((codelength%k) > 0)
	{
		blocknum = codelength / k + 1;
		b.resize(blocknum*k);
		for (int i = 0; i < codelength; i++)
			b[i] = a[i];
		for (int i = codelength; i < b.size(); i++)
			b[i] = 0;

	}
	else
	{
		blocknum = codelength / k;
		b.resize(codelength);
		b = a;
	}
	return b;

}


Vector<int> BchEncoder::bch_encode(Vector<int>data, int n, int k)
/*
r(x)=x^(n-k)*m(x)mod g(x);
C(x)=x^(n-k)*m(x)+r(x);

msg[]:the coeffients of m(x)
rr[]:r(x)
*/
{
	int    i, j;
	int    feedback;
	Vector<int>c(n);

	for (i = 0; i < n - k; i++)
		bb[i] = 0;
	for (i = k - 1; i >= 0; i--) {
		feedback = data[i] ^ bb[n - k - 1];
		if (feedback != 0) {
			for (j = n - k - 1; j > 0; j--)
				if (g[j] != 0)
					bb[j] = bb[j - 1] ^ feedback;
				else
					bb[j] = bb[j - 1];
			bb[0] = g[0] && feedback;
		}
		else {
			for (j = n - k - 1; j > 0; j--)
				bb[j] = bb[j - 1];
			bb[0] = 0;
		}
	}

	for (i = 0; i < n - k; i++)
		recd[i] = bb[i];
	for (i = 0; i < k; i++)
		recd[i + n - k] = data[i];

	for (int i = 0; i < n; i++)
		c[i] = recd[i];
	return c;
}