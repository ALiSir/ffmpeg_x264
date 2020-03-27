/*
 This program is distributed under the terms of the &apos;MIT license&apos;. The text
 of this licence follows...

 Copyright (c) 2004 J.D.Medhurst (a.k.a. Tixy)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef G711_H
#define G711_H

#include <stdint.h>

/*
class G711
{
public:
	static uint8_t  ALawEncode(int16_t pcm16);
	static int      ALawDecode(uint8_t alaw);
	static uint8_t  ULawEncode(int16_t pcm16);
	static int      ULawDecode(uint8_t ulaw);
	static uint8_t  ALawToULaw(uint8_t alaw);
	static uint8_t  ULawToALaw(uint8_t ulaw);
	static unsigned ALawEncode(uint8_t* dst, int16_t* src, size_t srcSize);
	static unsigned ALawDecode(int16_t* dst, const uint8_t* src, size_t srcSize);
	static unsigned ULawEncode(uint8_t* dst, int16_t* src, size_t srcSize);
	static unsigned ULawDecode(int16_t* dst, const uint8_t* src, size_t srcSize);
	static unsigned ALawToULaw(uint8_t* dst, const uint8_t* src, size_t srcSize);
	static unsigned ULawToALaw(uint8_t* dst, const uint8_t* src, size_t srcSize);
};
*/

uint8_t  ALawEncode(int16_t pcm16);
int      ALawDecode(uint8_t alaw);
uint8_t  ULawEncode(int16_t pcm16);
int      ULawDecode(uint8_t ulaw);
uint8_t  ALawToULaw(uint8_t alaw);
uint8_t  ULawToALaw(uint8_t ulaw);
unsigned int ALawEncode_len(uint8_t* dst, int16_t* src, size_t srcSize);
unsigned int ALawDecode_len(int16_t* dst, const uint8_t* src, size_t srcSize);
unsigned int ULawEncode_len(uint8_t* dst, int16_t* src, size_t srcSize);
unsigned int ULawDecode_len(int16_t* dst, const uint8_t* src, size_t srcSize);
unsigned int ALawToULaw_len(uint8_t* dst, const uint8_t* src, size_t srcSize);
unsigned int ULawToALaw_len(uint8_t* dst, const uint8_t* src, size_t srcSize);
#endif
