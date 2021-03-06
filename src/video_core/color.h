// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "common/common_types.h"
#include "video_core/math.h"

namespace Color {

/// Convert a 1-bit color component to 8 bit
inline u8 Convert1To8(u8 value) {
    return value * 255;
}

/// Convert a 4-bit color component to 8 bit
inline u8 Convert4To8(u8 value) {
    return (value << 4) | value;
}

/// Convert a 5-bit color component to 8 bit
inline u8 Convert5To8(u8 value) {
    return (value << 3) | (value >> 2);
}

/// Convert a 6-bit color component to 8 bit
inline u8 Convert6To8(u8 value) {
    return (value << 2) | (value >> 4);
}

/// Convert a 8-bit color component to 1 bit
inline u8 Convert8To1(u8 value) {
    return value >> 7;
}

/// Convert a 8-bit color component to 4 bit
inline u8 Convert8To4(u8 value) {
    return value >> 4;
}

/// Convert a 8-bit color component to 5 bit
inline u8 Convert8To5(u8 value) {
    return value >> 3;
}

/// Convert a 8-bit color component to 6 bit
inline u8 Convert8To6(u8 value) {
    return value >> 2;
}

/**
 * Decode a color stored in RGBA8 format
 * @param bytes Pointer to encoded source color
 * @return Result color decoded as Math::Vec4<u8>
 */
inline const Math::Vec4<u8> DecodeRGBA8(const u8* bytes) {
    return { bytes[3], bytes[2], bytes[1], bytes[0] };
}

/**
 * Decode a color stored in RGB8 format
 * @param bytes Pointer to encoded source color
 * @return Result color decoded as Math::Vec4<u8>
 */
inline const Math::Vec4<u8> DecodeRGB8(const u8* bytes) {
    return { bytes[2], bytes[1], bytes[0], 255 };
}

/**
 * Decode a color stored in RGB565 format
 * @param bytes Pointer to encoded source color
 * @return Result color decoded as Math::Vec4<u8>
 */
inline const Math::Vec4<u8> DecodeRGB565(const u8* bytes) {
    const u16_le pixel = *reinterpret_cast<const u16_le*>(bytes);
    return { Convert5To8((pixel >> 11) & 0x1F), Convert6To8((pixel >> 5) & 0x3F),
        Convert5To8(pixel & 0x1F), 255 };
}

/**
 * Decode a color stored in RGB5A1 format
 * @param bytes Pointer to encoded source color
 * @return Result color decoded as Math::Vec4<u8>
 */
inline const Math::Vec4<u8> DecodeRGB5A1(const u8* bytes) {
    const u16_le pixel = *reinterpret_cast<const u16_le*>(bytes);
    return { Convert5To8((pixel >> 11) & 0x1F), Convert5To8((pixel >> 6) & 0x1F),
        Convert5To8((pixel >> 1) & 0x1F), Convert1To8(pixel & 0x1) };
}

/**
 * Decode a color stored in RGBA4 format
 * @param bytes Pointer to encoded source color
 * @return Result color decoded as Math::Vec4<u8>
 */
inline const Math::Vec4<u8> DecodeRGBA4(const u8* bytes) {
    const u16_le pixel = *reinterpret_cast<const u16_le*>(bytes);
    return { Convert4To8((pixel >> 12) & 0xF), Convert4To8((pixel >> 8) & 0xF),
        Convert4To8((pixel >> 4) & 0xF), Convert4To8(pixel & 0xF) };
}

/**
 * Encode a color as RGBA8 format
 * @param color Source color to encode
 * @param bytes Destination pointer to store encoded color
 */
inline void EncodeRGBA8(const Math::Vec4<u8>& color, u8* bytes) {
    bytes[3] = color.r();
    bytes[2] = color.g();
    bytes[1] = color.b();
    bytes[0] = color.a();
}

/**
 * Encode a color as RGB8 format
 * @param color Source color to encode
 * @param bytes Destination pointer to store encoded color
 */
inline void EncodeRGB8(const Math::Vec4<u8>& color, u8* bytes) {
    bytes[2] = color.r();
    bytes[1] = color.g();
    bytes[0] = color.b();
}

/**
 * Encode a color as RGB565 format
 * @param color Source color to encode
 * @param bytes Destination pointer to store encoded color
 */
inline void EncodeRGB565(const Math::Vec4<u8>& color, u8* bytes) {
    *reinterpret_cast<u16_le*>(bytes) = (Convert8To5(color.r()) << 11) |
        (Convert8To6(color.g()) << 5) | Convert8To5(color.b());
}

/**
 * Encode a color as RGB5A1 format
 * @param color Source color to encode
 * @param bytes Destination pointer to store encoded color
 */
inline void EncodeRGB5A1(const Math::Vec4<u8>& color, u8* bytes) {
    *reinterpret_cast<u16_le*>(bytes) = (Convert8To5(color.r()) << 11) |
        (Convert8To5(color.g()) << 6) | (Convert8To5(color.b()) << 1) | Convert8To1(color.a());
}

/**
 * Encode a color as RGBA4 format
 * @param color Source color to encode
 * @param bytes Destination pointer to store encoded color
 */
inline void EncodeRGBA4(const Math::Vec4<u8>& color, u8* bytes) {
    *reinterpret_cast<u16_le*>(bytes) = (Convert8To4(color.r()) << 12) |
        (Convert8To4(color.g()) << 8) | (Convert8To4(color.b()) << 4) | Convert8To4(color.a());
}

} // namespace
