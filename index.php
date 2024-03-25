<?php

// function checkCookie($value)
// {
// 	$date = 0;
// 	//recuperer la date d'expiration dans la value
// 	if (date > time())
// 		return (0);
// 	else
// 		return (1);
// }

function getLogged($test)
{
	fwrite(STDERR, "//////////////////////////////////////////////// \n");
	fwrite(STDERR, "test = $test \n");
	fwrite(STDERR, "cookie = " . getenv('HTTP_COOKIE') . "\n");
	fwrite(STDERR, "//////////////////////////////////////////////// \n");
	$date = 0;
	$name = '';
	$cookie = '';
	$firstCookie = '';

	if (!empty(getenv('HTTP_COOKIE')))
	{
		$firstCookie = explode(';', getenv('HTTP_COOKIE'))[0];
		$info = explode('|', $firstCookie);
		foreach (explode('|', $firstCookie) as $value) {
			fwrite(STDERR, "val = $value \n");
		}
		if (!isset($info[1]) && empty($test))
		{
			fwrite(STDERR, "it's ////////////////\n");
			return getIndex();
		}
		else
		{
			fwrite(STDERR, "it's not ////////////////\n");
			$date = $info[1];
			fwrite(STDERR, "info[0] = $info[0] \n");
			foreach ( explode('=' ,$info[0]) as $value) {
				fwrite(STDERR, "val = $value \n");
			}
			$info = explode('=' ,$info[0]);
			fwrite(STDERR, "info[0] = $info[0] \n");
			$cookie = $info[1];
			$name = $info[1];
			fwrite(STDERR, "date == " . $date . "\n");
			fwrite(STDERR, "cookie == ". $cookie . "\n");
			fwrite(STDERR, "name == ". $name . "\n");
		}

	}
	// if (!empty(getenv('HTTP_COOKIE')))
	// {
	// 	fwrite(STDERR, "cookie = " . getenv('HTTP_COOKIE') . "\n");
	// 	fwrite(STDERR, "cookie!!!!!!! = $firstCookie \n");
	// 	$res = explode(' ', $firstCookie)[0];
	// 	$name = explode('=', $res)[1];
	// 	$cookie = $name;
	// 	fwrite(STDERR, "value 1 = $res \n");
	// 	$date = explode('=', explode(' ', $firstCookie)[1])[0];
	// 	//$date = explode('=', $date)[1];
	// 	fwrite(STDERR, "date = $date \n");
	// }
	if (!empty($test))
	{
		$cookie = explode('&', $test)[0];
		fwrite(STDERR, "cook = $cookie \n");
		$cookie = explode('=', $cookie)[1];
		$name = $cookie;
		fwrite(STDERR, "cook = $cookie \n");
	}
	$cookie = $date;
	fwrite(STDERR, "date = $date \n");
	fwrite(STDERR, "name = $name \n");
	fwrite(STDERR, "date = $date > " . (time() + 30 > $date ? 'true' : 'false') . "\n");
	$dateCount = $date?($date - time()):30;
	if (empty($name))
		return getIndex("<div>please enter an username</div>");
	if ($date && $dateCount < 0)
	{
		fwrite(STDERR, "il est passe par ici\n");
		return getIndex("<div>invalid request, please try again</div>");
	}
	else
	{
		$body = '<!DOCTYPE html>
		<html lang="fr">
		<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Formulaire de Connexion</title>
		<style>
		body
		{
			background-color: #ffffff;
			margin: 0;
			padding: 0;
			display: flex;
			justify-content: center;
			align-items: center;
			height: 100vh;
		}
		.container
		{
			display: flex;
			justify-content: center;
			align-items: center;
			background-color: #f2f2f2;
			width: 75%;
			height: 25%;
			border-radius: 10px;
			padding: 20px;
			flex-direction: column;
			box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
		}
		h2
		{
			margin-top: 0;
			margin-bottom: 10px;
		}
		</style>
		</head>
		<body>
		<div class="container">
		<h2>hello ';
		$body.=$name;
		fwrite(STDERR, "date = $date \n");
		fwrite(STDERR, "date = $date \n");
		$body.='</h2><p>still connected for '. $dateCount .' s</p></body></html>';
		//fwrite(STDERR, "time???? = $date\n");
		//fwrite(STDERR, "time???? = " . time()+30 . "\n");
		//fwrite(STDERR, "\n\n\nheader = $header \n\n\n");
		$date = ($date ? $date : time() + 30);
		$header = "HTTP/1.1 200 OK\r\n";
		$header .= "Allow: GET\r\n";
		$header .= "Content-Type: text/html; charset=utf-8\r\n";
		$header .= "Set-Cookie: auth=" . $name . '|' . $date . "; expires=" . gmdate('D, d M Y H:i:s \G\M\T', $date) . ";\r\n path=/index.php\r\n";
		$header .= "Content-Length: " . strlen($body) . "\r\n\r\n";
		fwrite(STDERR, "header == $header");
		return ($header.$body);
	}
}

function getIndex($ARG='')
{
	$body = '<!DOCTYPE html>
	<html lang="fr">
	<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Formulaire de Connexion</title>
	<style>
	body {
		background-color: #ffffff;
		margin: 0;
		padding: 0;
		display: flex;
		justify-content: center;
		align-items: center;
		height: 100vh;
	}
	.container {
		background-color: #f2f2f2;
		width: 75%;
		height: 25%;
		border-radius: 10px;
		padding: 20px;
		box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
	}
	input[type="text"],
	input[type="password"] {
		width: 100%;
		padding: 10px;
		margin-bottom: 10px;
		border-radius: 5px;
		border: 1px solid #ccc;
		box-sizing: border-box;
	}
	input[type="submit"] {
		width: 100%;
		padding: 10px;
		border-radius: 5px;
		border: none;
		background-color: #4CAF50;
		color: white;
		cursor: pointer;
	}
	h2 {
		margin-top: 0;
		margin-bottom: 10px;
	}
	</style>
	</head>
	<body>
	<div class="container">
	<form method="post">';
	if (!empty($ARG))
		$body .= $ARG;
	$body .= '<h2>Username</h2>
		<input type="text" name="username" id="username" placeholder="Entrez votre nom d\'utilisateur">

		<h2>Password</h2>
		<input type="password" name="password" id="password" placeholder="Entrez votre mot de passe">

		<input type="submit" value="Connexion">
	</form>
	</div></body>
	</html>';
	$header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\ncharset=utf-8\r\n";
	// if (!empty($ARG))
	// {
	// 	fwrite(STDERR, "iciiiiiiiiiiiiiiiiiiiii\n");
	// 	fwrite(STDERR, "ARG = $ARG" );
	// 	$header .= "Set-Cookie: auth= ;";
	// 	$header .= "expires=" . (time() - 36000) . ";\r\n";
	// }
	$header .= "Content-Length: " . strlen($body) . "\r\n\r\n";
	fwrite(STDERR, "header == $header");
	return ($header.$body."\n");
}

// <div class="container">';
	// if ($ARG)
	// 	$body .= $ARG;
	// $body .= '<h2>Username</h2>
	// <input type="text" name="username" id="username" placeholder="Entrez votre nom d\'utilisateur">

	// <h2>Password</h2>
	// <input type="password" name="password" id="password" placeholder="Entrez votre mot de passe">

	// <input type="submit" value="Connexion">
	// </div>

function main()
{
	// foreach ($_ENV as $key => $value) {
	// 	echo "$key : $value <br>";
	// }
//	$test = getenv();
//	foreach ($test as $value) {
//		fwrite(STDERR, "value = $value");
//	}
	$truc = '';
	// fwrite(STDERR, "?????????????????\n");
	foreach ($_ENV as $key => $value) {
		fwrite(STDERR, "Clé: $key, Valeur: $value\n");
	}

	$in = file_get_contents('php://stdin');
	// while (!feof(STDIN)) {
	// 	$in .= fgets(STDIN);
	// }
		if (!empty($in) && $in[strlen($in) - 1] == "\n")
		$in = substr($in, 0, -1);
	if (!getenv('HTTP_COOKIE') && empty($in))
		echo getIndex();
	else
		echo getLogged($in);
}

main();

?>
