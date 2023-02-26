//
// Created by calt on 18-10-18.
//

#ifndef ROI_TOOLLET_H
#define ROI_TOOLLET_H

#include <Util/Math/Convert/Convert.h>
#include <QtGui/QImage>
#include <Config/Config.h>

/**
 * image xor
 * @param origin_img
 * @param key
 * @param use_origin: if use_origin == true, then use origin to as last; or not, use xor to as last;
 *        usage: when encryption, use_origin should be false; when decryption, use_orgin should be true
 * @return
 */
QImage gray_xor(const QImage &origin_img, const std::string &key, bool use_origin, const QVector<QRgb> &gray_color_table);

QImage substitute(const QImage &origin_img, const std::string &key, const QVector<QRgb> &gray_color_table, bool reverse);

#endif //ROI_TOOLLET_H
