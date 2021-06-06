basdon-fly
----------

basdon.net aviation server - A GTA:San Andreas Multiplayer (SA-MP) server.

This repository contains almost everything(*) needed to run the complete
basdon-fly server and services package.

Maps and textdraw resource files are not uploaded (yet?).

The first commit was made 17 Jun 2018 in the fly-gm gamemode repository.
The fly-plugin plugin development started 22 Sep 2018.
The fly-plugin plugin has replaced the fly-gm gamemode since 3 Dec 2019.

The server has been live since 25 Dec 2019 at
https://basdon.net and samp://basdon.net:7777

This project is licensed under the GNU AGPLv3 License.
Files in /fly-plugin/vendor/ do NOT fall under this License.
Files in /fly-plugin/vendor/SDK/amx are Copyright (c) ITB CompuPhase, 1997-2005
Files in /fly-plugin/vendor/SDK (excluding the amx/ directory) are Copyright 2004-2009 SA-MP Team
Modifications in these files are marked as such with comments.

Anatomy
-------

This repository is a merge of now-archived repositories:
basdon/fly-db
basdon/fly-plugin
basdon/fly-web
basdon/fly-services (now fly-echo and fly-flighttracker)

Since all those files were moved into subdirectories, their history/blame might
not be representative. Check out the archived repositories to see the correct
history/blame.

fly-db: files with SQL queries that make up and change the database structure
fly-echo: Anna^ module(*) providing chat link between in-game and IRC
fly-flighttracker: Anna^ module(*) keeping track of active flights
fly-plugin: SA:MP plugin being the gamemode logic
fly-web: the website
maps: map files (.map) go in here, these define clusters of objects
server: server configuration files
textdraws: textdraw files (.text) go in here, these define textdraws

Each of those may have another readme.txt file with extra information.

(*) fly-echo and fly-flighttracker are Anna^ modules.
Anna^ is a modular IRC bot and can be found at https://github.com/yugecin/anna.

Building
--------

	Requirements
	============
	all: make, bash
	fly-echo: javac, anna (https://github.com/yugecin/anna)
	fly-flighttracker: javac, anna (https://github.com/yugecin/anna)
	fly-plugin: a Linux system with gcc, nasm, tcc
	fly-web: sed, php with extensions pdo_mysql,gd,mbstring

	For fly-echo and fly-flighttracker to be built, the environment
	variable $ANNA_DIR needs to be set to the location where the
	anna repository (https://github.com/yugecin/anna) is cloned.

	fly-echo and fly-flighttracker are optional and will not be built
	when the $ANNA_DIR environment variable is not set or javac is not
	found. A warning will be printed but the build will not be aborted.

	Building
	========
	Run:

		make

	This will make everything. To only make one target, run for example:

		make plugin

	Available targets are: echo, flighttracker, plugin, web

	When building the plugin, a dev build is made if the $BASDONDEV env
	variable is present.

	Building the website means it will parse and generate all templates,
	articles, images, etc. In dev, the $BASDONDEV env variable should be
	set so file permissions are set so that the website can regenerate
	template files.

	Cleaning
	========
	Cleaning only applies to fly-echo and fly-flighttracker. Everything else
	is always rebuilt so there's no real need to clean.
	Run:

		make clean

Installation/configuration
--------------------------

	Requirements
	============
	mysql/mariadb: import from sql files in the fly-db folder
	fly-echo,fly-flighttracker: anna (github.com/yugecin/anna)
	fly-web: php with extensions pdo_mysql,sockets
	server: other plugins:
		bcrypt-samp v2.2 (github.com/lassir/bcrypt-samp)
		mysql r39.3 (github.com/pBlueG/SA-MP-MySQL)
		simplesocket (github.com/basdon/simplesocket) (to be embedded)

	Download the SA:MP server package at https://sa-mp.com/download.php

	Configuration
	=============
	general:
		The settings.ini file in the root dir controls various settings
		that are used both in the fly-plugin and fly-web code.
		Changes made in this file will only be applied after rebuilding
		plugin and web (run 'make').

	anna:
		For chatlogs, enable the packaged mod_log and set the path.
		For example, if the IRC channel is #basdon and the echo channel
		is #basdon.echo, add this to
		$ANNA_DIR/out/config/mod_log.properties:

			channel.basdon.echo=/path/to/basdon-fly/fly-web/static/chatlogs/basdon.echo
			channel.basdon=/path/to/basdon-fly/fly-web/static/chatlogs/basdon

	      Or set it by sending commands to Anna^:

			&confadd mod_log channel.basdon.echo /path/to/basdon-fly/fly-web/static/chatlogs/basdon.echo
			&confadd mod_log channel.basdon /path/to/basdon-fly/fly-web/static/chatlogs/basdon

	fly-echo:
		The mod_bas_echo.jar file is placed in $ANNA_DIR/out by the
		build script. Enable and load it by sending the command:

			&enablemod mod_bas_echo

		Add this to $ANNA_DIR/out/config/mod_bas_echo.properties:

			echo.channel=\#basdon.echo

		Or set it by sending commands to Anna^:

			&confadd mod_bas_echo echo.channel #basdon.echo

		Anna^ will join the channel when the module is loaded. If the
		setting was not set upon load, either reload the module or use
		the &join command to make Anna^ join the channel.

	fly-flighttracker:
		The mod_bas_ft.jar file is placed in $ANNA_DIR/out by the
		build script. Enable and load it by sending the command:

			&enabledmod mod_bas_ft

		Add this to $ANNA_DIR/out/config/mod_bas_ft.properties:

			fdr.path=/path/to/basdon-fly/fly-web/static/fdr

	      Or set it by sending commands to Anna^:

			&confadd mod_bas_ft fdr.path /path/to/basdon-fly/fly-web/static/fdr

	fly-web:
		An Apache http server configuration file is provided in
		fly-web/apache.conf, it should be straight-forward enough to
		see what it does in case you use a different webserver.

		Copy fly-web/inc/conf.sample.php to fly-web/inc/conf.php and
		edit as needed.

		Add a cron job to regenerate the player graph image every
		5 minutes:

			*/5 * * * * cd /path/to/basdon-fly/fly-web/cli; php genplayergraph.php

		In dev, set the SGID sticky bit (g+s) on the fly-web/inc and
		fly-web/gen folders. Otherwise it won't be able to regenerate
		some files (including templates and settings files) on page load
		when the $__REPARSE__ setting is set. Also remember to run make
		with the $BASDONDEV environment variable so it will set g+w
		permissions on the files when (re)generating them from the
		Makefile.

	server:
		Copy server/conf.sample.ini to server/conf.ini and edit as
		needed.

		When running, the server.cfg file is created from combining:

			servercfg-common.cfg
			servercfg-rcon_password.cfg (generated on first run)
			servercfg-dev-*.cfg (when dev)
			servercfg-prod-*.cfg (when prod)

		Additional cfg files can be made, use the numbering (like
		in 00 for the default dev/prov cfg files) to ensure an order.
		Settings can be specified multiple times, SA-MP uses the last
		supplied one.

	Things to backup periodically
	=============================
	The database
	Flight files: /path/to/basdon-fly/fly-web/static/fdr/*
	Chatlogs: /path/to/basdon-fly/fly-web/static/chatlogs/*

Running
-------

Run Anna^. It will load the fly-echo and fly-flighttracker modules (when
configured correctly). This is not required to run the server, but it should be
avoided to run without fly-flighttracker as there will be no flightmaps on the
website for any flights done while it was not running.

Run the server by executing:

	. start

To use the dev configuration, set the $BASDONDEV environment variable.

Running the server without the dev settings will start it using nohup and run
it in the background.

Notes
-----

The gamemode script is a stub and is deprecated, and might be removed in
the future.
