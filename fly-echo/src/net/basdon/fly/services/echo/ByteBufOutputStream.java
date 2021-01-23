// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.io.OutputStream;

/**
 * This class is separate from ByteBuf because otherwise I'd get
 * a lot more complaints about unclosed resources.
 */
class ByteBufOutputStream extends OutputStream
{
private final ByteBuf delegate;

ByteBufOutputStream(ByteBuf buf)
{
	this.delegate = buf;
}

@Override
public
void write(int b)
{
	this.delegate.buf[this.delegate.pos++] = (byte) b;
}

@Override
public
void write(byte[] b, int off, int len)
{
	System.arraycopy(b, off, this.delegate.buf, this.delegate.pos, len);
	this.delegate.pos += len;
}
}
