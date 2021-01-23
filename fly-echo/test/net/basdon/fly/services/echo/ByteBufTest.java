// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

public
class ByteBufTest
{
@Test
public
void test_write_then_read_int()
{
	int num = 0x87654321; // most significant bit set on purpose
	byte pack_type = 1;

	ByteBuf pd = new ByteBuf(8, pack_type);
	pd.writeDword(num);

	pd = new ByteBuf(pd.buf);
	assertEquals('F', pd.readByte());
	assertEquals('L', pd.readByte());
	assertEquals('Y', pd.readByte());
	assertEquals(pack_type, pd.readByte());
	assertEquals(num, pd.readDword());
}
}
