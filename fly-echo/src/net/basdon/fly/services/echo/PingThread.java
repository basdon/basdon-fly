// Copyright 2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

class PingThread extends Thread
{
private final Echo echo;

PingThread(Echo echo)
{
	this.echo = echo;
}

@Override
public
void run()
{
	for (;;) {
		try {
			Thread.sleep(Echo.PING_INTERVAL);
		} catch (InterruptedException e) {
			return;
		}
		this.echo.anna.sync_exec(() -> {
			this.echo.send_ping();
			this.echo.check_ping_timeout();
		});
	}
}
}
