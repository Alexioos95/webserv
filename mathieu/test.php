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
	//if (!checkCookie($test))
	//	return getIndex("<div>invalid request, please try again</div>");
	//else
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
			display: flex;
			justify-content: center;
			align-items: center;
			background-color: #f2f2f2;
			width: 75%;
			height: 25%;
			border-radius: 10px;
			padding: 20px;
			box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
		}
		h2 {
			margin-top: 0;
			margin-bottom: 10px;
		}
		</style>
		</head>
		<body>
		<div class="container">
		<h2>hello ';
		if (isset($_ENV["HTTP_COOKIE"]))
			$body .= $_ENV[HTTP_COOKIE];
		else
			$body .= "Random";
		$body .= '</h2>
		</div>
		</body>
		</html>';
		$header = "HTTP/1.1 200; Content-Type: text/html; charset=utf-8; ";
		if (!empty($test))
		{
			$cookie = explode('&', $test);
			$header .= "Set-Cookie: ";
			$header .= "test=".$cookie[0] . ";";
			$header .= " expire=";
			$header .= time() + 30 . " ";
		}
		$header .= "Content-Length: " . strlen($body) . "\n";
		return ($header.$body);
	}
}

function getIndex($ARG=0)
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
	<div class="container">';
	if ($ARG)
		$body .= $ARG;
	$body .= '<h2>Username</h2>
	<input type="text" name="username" id="username" placeholder="Entrez votre nom d\'utilisateur">

	<h2>Password</h2>
	<input type="password" name="password" id="password" placeholder="Entrez votre mot de passe">

	<input type="submit" value="Connexion">
	</div>
	</body>
	</html>';
	$header = "HTTP/1.1 200; Content-Type: text/html; charset=utf-8; ";
	$header .= " Content-Length: " . strlen($body) . "\n";
	return ($header.$body."\n");
}

function main()
{
	echo "je suis dans du php \n";
	$in = file_get_contents('php://stdin');
	while (!feof(STDIN)) {
         $in .= fgets(STDIN);
    }
	if ($in != '' && $in[strlen($in) - 1] == "\n")
		$in = substr($in, 0, -1);
	if (!isset($_ENV["HTTP_COOKIE"]) && $in == '')
		echo getIndex();
	else
		echo getLogged($in);
}

main();

?>