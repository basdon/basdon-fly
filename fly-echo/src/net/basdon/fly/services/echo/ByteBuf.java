// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

/**
 * Like {@link java.nio.ByteBuffer}, but easier for bulk array copy ops.
 *
 * Both for reading and writing. Your responsability to use the correct constructor and methods.
 */
class ByteBuf
{
int pos;
int mark;
byte[] buf;

/**
 * @param len must be at least 4
 */
ByteBuf(int len, byte packet_id)
{
	this.buf = new byte[len];
	this.buf[0] = 'F';
	this.buf[1] = 'L';
	this.buf[2] = 'Y';
	this.buf[3] = packet_id;
	this.pos = 4;
}

ByteBuf(int len)
{
	this.buf = new byte[len];
	this.pos = 0;
}

ByteBuf(byte[] buf)
{
	this.buf = buf;
	this.pos = 0;
}

void skip(int len)
{
	this.pos += len;
}

void markAndSkip(int len)
{
	this.mark = this.pos;
	this.pos += len;
}

void exchangeMarkAndPos()
{
	int tmp = this.pos;
	this.pos = this.mark;
	this.mark = tmp;
}

void writeByte(byte data)
{
	this.buf[this.pos++] = data;

}
void writeByte(char data)
{
	this.buf[this.pos++] = (byte) data;
}

void writeWord(int data)
{
	this.buf[this.pos++] = (byte) (data & 0xFF);
	data >>>= 8;
	this.buf[this.pos++] = (byte) (data & 0xFF);
}

void writeDword(int data)
{
	this.buf[this.pos++] = (byte) (data & 0xFF);
	data >>>= 8;
	this.buf[this.pos++] = (byte) (data & 0xFF);
	data >>>= 8;
	this.buf[this.pos++] = (byte) (data & 0xFF);
	data >>>= 8;
	this.buf[this.pos++] = (byte) (data & 0xFF);
}

void copyFrom(ByteBuf src, int len)
{
	System.arraycopy(src.buf, src.pos, this.buf, this.pos, len);
	src.pos += len;
	this.pos += len;
}

void copyFrom(byte[] src, int len)
{
	System.arraycopy(src, 0, this.buf, this.pos, len);
	this.pos += len;
}

void writeString(String str)
{
	byte[] bytes = str.getBytes(StandardCharsets.UTF_8);
	this.copyFrom(bytes, bytes.length);
}

void writeASCII(char[] src, int offset, int len)
{
	while (len-- > 0) {
		this.buf[this.pos++] = (byte) src[offset++];
	}
}

/**
 * @return amount of bytes written
 */
int writeUTF8(char[] src, int offset, int len)
{
	int posBefore = this.pos;
	ByteBufOutputStream bbos = new ByteBufOutputStream(this);
	@SuppressWarnings("resource")
	PrintWriter pw = new PrintWriter(new OutputStreamWriter(bbos, StandardCharsets.UTF_8), false);
	pw.write(src, offset, len);
	pw.flush();
	return this.pos - posBefore;
}

byte readByte()
{
	return this.buf[this.pos++];
}

short readWord()
{
	int pos = this.pos += 2;
	short data = 0;
	data |= (0xFF & this.buf[--pos]);
	data <<= 8;
	data |= (0xFF & this.buf[--pos]);
	return data;
}

int readDword()
{
	int pos = this.pos += 4;
	int data = 0;
	data |= (0xFF & this.buf[--pos]);
	data <<= 8;
	data |= (0xFF & this.buf[--pos]);
	data <<= 8;
	data |= (0xFF & this.buf[--pos]);
	data <<= 8;
	data |= (0xFF & this.buf[--pos]);
	return data;
}
}
