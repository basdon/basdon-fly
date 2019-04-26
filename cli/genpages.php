<?php

chdir('../www');

require('../inc/spate.php');
spate_default_generate('index');
spate_default_generate('flights');
spate_default_generate('flight');
