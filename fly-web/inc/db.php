<?php

$db = null;
$db_querycount = 0;
try {
	$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
	$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->exec('SET NAMES utf8mb4');
	++$db_querycount;
} catch (PDOException $e) {
	$__msgs[] = "Cannot connect to db.";
}

/**
 * ensure PDO::ERRMODE_SILENT is set when using
 */
class DBQ
{
	public static $last_error;

	public $stmt;
	public $executed;
	/**may be SQLSTATE code or other string*/
	public $error;
	public $num_rows_fetched;

	function query($sql)
	{
		global $db, $db_querycount;

		$this->stmt = null;
		$this->executed = false;
		$this->error = null;
		self::$last_error = null;
		if (is_null($db)) {
			$this->set_error('', 'No db connection');
		} else {
			++$db_querycount;
			$this->stmt = $db->query($sql);
			if ($this->stmt === false) {
				$this->stmt = null;
				$this->set_error($db->errorCode(), 'Could not execute db query');
			} else {
				$this->executed = true;
			}
		}
		return $this;
	}

	function prepare($sql)
	{
		global $db;

		$this->stmt = null;
		$this->executed = false;
		$this->error = null;
		self::$last_error = null;
		if (is_null($db)) {
			$this->set_error('', 'No db connection');
		} else {
			$this->stmt = $db->prepare($sql);
			if ($this->stmt === false) {
				$this->stmt = null;
				$this->set_error($db->errorCode(), 'Could not setup db query');
			}
		}
		return $this;
	}

	/**bind zero indexed array*/
	function bind0ia($values)
	{
		if (!is_null($this->stmt)) {
			for ($i = count($values); $i > 0; $i--) {
				$this->stmt->bindValue($i, $values[$i - 1]);
			}
		}
		return $this;
	}

	function execute()
	{
		global $db_querycount;

		if (!is_null($this->stmt)) {
			$this->executed = false;
			$this->error = null;
			self::$last_error = null;
			$this->num_rows_fetched = 0;
			++$db_querycount;
			$this->executed = $this->stmt->execute();
			if (!$this->executed) {
				$this->set_error($this->stmt->errorCode(), 'Could not execute db query');
			}
		}
		return $this;
	}

	/**shorthand equivalent for $this->bind0ia($values)->execute()*/
	function executew0ia($values)
	{
		return $this->bind0ia($values)->execute();
	}

	function fetch_all()
	{
		if ($this->executed) {
			$this->error = null;
			self::$last_error = null;
			$res = $this->stmt->fetchAll();
			if ($res !== false) {
				$this->num_rows_fetched = count($res);
				return $res;
			}
			$this->set_error($stmt->errorCode(), 'Could not fetch db data');
		}
		return [];
	}

	function fetch_next()
	{
		if ($this->executed) {
			$this->error = null;
			self::$last_error = null;
			$res = $this->stmt->fetch();
			if ($res !== false) {
				$this->num_rows_fetched++;
				return $res;
			}
			$errorCode = $this->stmt->errorCode();
			if (!empty($errorCode) && $errorCode != '00000') {
				$this->set_error($stmt->errorCode(), 'Could not fetch db data');
			}
		}
		return false;
	}

	function fetch_generator()
	{
	next:
		$res = $this->fetch_next();
		if ($res !== false) {
			yield $res;
			goto next;
		}
	}

	private function set_error($code, $human)
	{
		if (empty($code)) {
			$this->error = $human;
		} else {
			$this->error = "{$human} ({$code})";
		}
		self::$last_error = $this->error;
	}
}

function dbquery($query)
{
	return (new DBQ())->query($query);
}
