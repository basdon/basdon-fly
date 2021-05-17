// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/**
 * Like {@link java.nio.CharBuffer}, but easier for bulk array copy ops.
 */
class CharBuf
{
static Charset cp1252;

static
{
	try {
		cp1252 = Charset.forName("Windows-1252");
	} catch (Throwable t) {
		t.printStackTrace();
		System.err.println("using ISO-8859-1 instead of Windows-1252");
		cp1252 = StandardCharsets.ISO_8859_1;
	}
}

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

void writeCP1252(byte[] from, int offset, int len)
{
	String str = new String(from, offset, len, cp1252);
	len = str.length();
	str.getChars(0, len, this.chars, this.pos);
	this.pos += len;
}

void writeCP1252(ByteBuf from, int len)
{
	this.writeCP1252(from.buf, from.pos, len);
	from.pos += len;
}

/**
 * Replaces every character lower than space with a space.
 *
 * Intended to remove \r and \n and control chars for colors etc.
 */
void writeCP1252Filtered(ByteBuf from, int len)
{
	int start = this.pos;
	this.writeCP1252(from, len);
	while (start < this.pos) {
		if (this.chars[start] <= ' ') {
			this.chars[start] = ' ';
		}
		start++;
	}
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
