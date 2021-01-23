// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.nio.charset.StandardCharsets;

/**
 * Like {@link java.nio.CharBuffer}, but easier for bulk array copy ops.
 */
class CharBuf
{
int pos;
char[] chars;

CharBuf(int len)
{
	this.chars = new char[len];
	this.pos = 0;
}

void writeChar(char data)
{
	this.chars[this.pos++] = data;
}

void writeUTF8(byte[] from, int offset, int len)
{
	String str = new String(from, offset, len, StandardCharsets.UTF_8);
	len = str.length();
	str.getChars(0, len, this.chars, this.pos);
	this.pos += len;
}

void writeUTF8(ByteBuf from, int len)
{
	this.writeUTF8(from.buf, from.pos, len);
	from.pos += len;
}

/**
 * Replaces every character lower than space with a space.
 *
 * Intended to remove \r and \n and control chars for colors etc.
 */
void writeUTF8Filtered(ByteBuf from, int len)
{
	int start = this.pos;
	this.writeUTF8(from, len);
	while (start < this.pos) {
		if (this.chars[start] <= ' ') {
			this.chars[start] = ' ';
		}
		start++;
	}
}

void writeString(String str)
{
	int len = str.length();
	str.getChars(0, len, this.chars, this.pos);
	this.pos += len;
}
}
