// Copyright 2019 basdon.net - this source is licensed under GPL
// see the LICENSE file for more details
package net.basdon;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;

public class Log implements AutoCloseable
{
	private static Log log;
	private static final long startTime = System.currentTimeMillis();

	public static long runtime()
	{
		return System.currentTimeMillis() - startTime;
	}

	private static void metalog(String entry)
	{
		log.fileOut.println(entry);
		System.out.println(entry);
	}

	public static void error(String message, Throwable e)
	{
		synchronized (log) {
			final String msg = String.format("[%8d] ERROR: %s", runtime(), message);
			System.err.println(msg);
			log.fileOut.println(msg);
			log.printStackTrace(e, System.err, "[        ] ERROR: ");
		}
	}

	public static void error(Throwable e)
	{
		error("", e);
	}

	public static void error(String message)
	{
		synchronized (log) {
			final String msg = String.format("[%8d] ERROR: %s", runtime(), message);
			System.err.println(msg);
			log.fileOut.println(msg);
		}
	}

	public static void warn(String message)
	{
		synchronized(log) {
			final String msg = String.format("[%8d] WARN: %s", runtime(), message);
			System.out.println(msg);
			log.fileOut.println(msg);
		}
	}

	public static void warn(String message, Throwable e)
	{
		synchronized (log) {
			final String msg = String.format("[%8d] WARN: %s", runtime(), message);
			System.out.println(msg);
			log.fileOut.println(msg);
			log.printStackTrace(e, System.out, "[        ] WARN: ");
		}
	}

	public static void info(String message)
	{
		synchronized (log) {
			final String msg = String.format("[%8d] INFO: %s", runtime(), message);
			System.out.println(msg);
			log.fileOut.println(msg);
		}
	}

	public static void debug(String message)
	{
		synchronized (log) {
			final String msg = String.format("[%8d] DEBUG: %s", runtime(), message);
			System.out.println(msg);
			log.fileOut.println(msg);
		}
	}

	private final PrintStream fileOut;

	public Log()
	{
		if (log != null) {
			fileOut = null;
			return;
		}
		log = this;

		FileOutputStream fout = null;
		try {
			fout = new FileOutputStream("log.txt", /*append*/ true);
		} catch (Throwable t) {
			System.err.println("Could not open log file, logging will be console only");
			t.printStackTrace();
		}

		if (fout != null) {
			this.fileOut = new PrintStream(fout, /*autoFlush*/ true);
		} else {
			this.fileOut = new PrintStream(new OutputStream() {
				@Override
				public void write(int b) throws IOException
				{
				}
			}, /*autoFlush*/ true);
		}

		metalog("\nSession Start: " + new Date().toString());
	}

	@Override
	public void close()
	{
		metalog("Session Close: " + new Date().toString());
		this.fileOut.close();
	}

	private void printStackTrace(Throwable t, PrintStream primaryOutput, String prefix)
	{
		final StringWriter sw = new StringWriter();
		final PrintWriter pw = new PrintWriter(sw);
		t.printStackTrace(pw);
		pw.flush();
		final StringBuffer buf = sw.getBuffer();
		final char[] chars = new char[buf.length()];
		buf.getChars(0, chars.length, chars, 0);
		primaryOutput.print(prefix);
		this.fileOut.print(prefix);
		for (int i = 0; i < chars.length;) {
			char c = chars[i++];
			if (c == '\r') {
				if (i < chars.length && chars[i] == '\n') {
					continue;
				}
				c = '\n';
			}
			if (c == '\n') {
				primaryOutput.print('\n');
				this.fileOut.print('\n');
				if (i < chars.length) {
					primaryOutput.print(prefix);
					this.fileOut.print(prefix);
					if (chars[i] == '\t') {
						chars[i] = ' ';
					}
				}
				continue;
			}
			primaryOutput.print(c);
			this.fileOut.print(c);
		}
		primaryOutput.flush();
		this.fileOut.flush();
	}
}