# RRFRemote
![basic](https://img.shields.io/badge/M5Stack-BASIC-blue)
![grey](https://img.shields.io/badge/M5Stack-GREY-blue)
![fire](https://img.shields.io/badge/M5Stack-FIRE-orange)
![core2](https://img.shields.io/badge/M5Stack-CORE2-green)
![aws](https://img.shields.io/badge/M5Stack-AWS-orange)

![ATOM Lite](https://img.shields.io/badge/M5Stack-ATOM%20Lite-darkgrey)
![ATOM Echo Smart](https://img.shields.io/badge/M5Stack-ATOM%20Echo%20Smart-white)
![ATOM Matrix](https://img.shields.io/badge/M5Stack-ATOM%20Matrix-blue)

![licence](https://img.shields.io/github/license/armel/RRFRemote)
![language](https://img.shields.io/github/languages/top/armel/RRFRemote)
![size](https://img.shields.io/github/repo-size/armel/RRFRemote)
![version](https://img.shields.io/github/v/release/armel/RRFRemote)
![activity](https://img.shields.io/github/commit-activity/y/armel/RRFRemote)

Le projet RRFRemote permet de suivre l'activité du RRF et de piloter son point d'accès ou hotpsot. Il peut fonctionner en charge (via un connecteur USB) ou sur batterie. 

![RRFRemote](https://github.com/armel/RRFRemote/blob/main/img/RRFRemote.jpeg)

**Profiter de 5% de réduction, sur l'ensemble de la boutique M5Stack en utilisant le code de réduction F4HWN ou en cliquant directement sur [M5Stack](https://shop.m5stack.com/?ref=LUxetaH4).**

# Architecture technique

## Présentation rapide

[M5Stack](https://m5stack.com/) est basé sur un ESP-32, dual core, pouvant être cadencé jusqu'à 240 MHz.  Néanmoins, le RRFRemote fonctionne parfaitement à 80 MHz. M5Stack dispose de 16 MB de mémoire flash. Comme tous les ESP, le Wifi est évidement intégré. L'écran 2 pouces IPS couleur, basé sur le chipset ILI9342C, affiche une résolution confortable de 320 x 240 pixels. Il est hyper lumineux. La batterie intégrée fait 110 mAh. Il est possible d'ajouter une batterie supplémentaire (de 700 ou 800mAh) si besoin. 

En terme de dimensions et de masse, c'est très compact : 54 x 54 x 18mm pour 47,2g. Se trimbale dans la poche sans problème ;) 

## Specs techniques détaillées :

Voici les specs techniques détaillées, pour les curieux :

| Resources |	Description |
| --------- | ------------ |
|ESP32| 240MHz dual core, 600 DMIPS, 520KB SRAM, Wi-Fi, dual mode Bluetooth
Flash| Memory	16MB|
|Power| Input	5V @ 500mA|
|Port|	TypeC x 1, GROVE(I2C+I/0+UART) x 1|
|Core|Bottom Port	PIN (G1，G2，G3，G16, G17, G18, G19, G21, G22, G23, G25, G26, G35, G36)|
|IPS Screen|	2 inch, 320x240 Colorful TFT LCD, ILI9342C, max brightness 853nit|
|Button|	Custom button x 3|
|Speaker|	1W-0928|
|Battery|	110mAh @ 3.7V|
|Antenna|	2.4G 3D Antenna|
|Operating Temperature|	32°F to 104°F ( 0°C to 40°C )|
|Net weight|	47.2g|
|Gross weight|	93g|
|Product Size|	54 x 54 x 18mm|
|Package Size	|95 x 65 x 25mm|
|Case Material|	Plastic ( PC )|

## En complément

Coté QSJ, compter autour de 35€. Vous disposez alors d'une plateforme de développement complète, totalement autonome, programmable en C et C++, MicroPython et UIFlow, depuis Linux, Windows ou MacOS, le tout dans un boitier compact et ultra ergonomique.

Ce QSJ est à comparer à celui d'un écran Nextion type NX4832K035, neurasthénique, dépourvu de connectivité Wifi ou GPIO, programmable sous un environnement totalement propriétaire, impliquant un cablage et incapable de fonctionner en standalone. Bref, pour reprendre une des formules du fabriquant, un écran Nextion **_est la meilleure solution pour remplacer les tubes LED classiques Nixie_**. Et nous sommes d'accord, HI ;) 

## Versions supportées actuellement

Le développement actuel du RRFRemote prend en charge le [M5Stack Basic](https://m5stack.com/collections/m5-core/products/basic-core-iot-development-kit), le [M5Stack Grey](https://m5stack.com/collections/m5-core/products/grey-development-core), le [M5Stack Fire](https://shop.m5stack.com/collections/m5-controllers/products/m5stack-fire-iot-development-kit-psram-v2-6)  ainsi que les [M5Stack Core2](https://m5stack.com/products/m5stack-core2-esp32-iot-development-kit) et [M5Stack AWS](https://shop.m5stack.com/collections/m5-controllers/products/m5stack-core2-esp32-iot-development-kit-for-aws-iot-edukit). 

En complément, il est désormais possible de faire fonctionner le RRFRemote avec l'[ATOM Display](https://shop.m5stack.com/products/m5stack-atom-psram-lcd-display-driver-kit) et profiter d'un écran plus grand en utilisant la sortie HDMI. Je vous recommande, en complément, de procéder à l'acquisition de l'unitié [Mini Dual Button](https://shop.m5stack.com/products/mini-dual-button-unit), dont le QSJ est inférieur à 3€, afin de pouvoir profiter de 2 boutons supplémentaire, en plus du bouton unique présent sur l'ATOM, afin de pouvoir utiliser l'ATOM comme un M5Stack à bouton.

> Vous devez néanmoins disposer d'un écran supportant la résolution 320 x 240 pixels.

# Fonctionnalités

Au stade actuel du développement, le RRFRemote propose les fonctionnalités suivantes.

## Concernant le suivi de l'activité du RRF
Voici la liste des fonctionnalités actuelles :

- suivi _quasi_ temps réel de l'activité des salons du RRF,
- bascule de salon en salon, via les boutons gauche et droit,
- affichage de l'indicatif du link en cours d'émission et de son temps d'émission,
- affichage alterné :
	- du nombre de links connectés,
	- du nombre de links actifs,
	- du nombre de passages en émission depuis le début de la journée,
	- du temps d'émission cumulé depuis le début de la journée,
	- de l'heure,
	- de la distance avec l'ISS,
- affichage des links entrants et sortants, via scrolling horizontal,
- affichage du département, du locator et de la distance du link en cours d'émission, via scrolling horizontal,
- affichage alterné complémentaire :
	- du top 10 des links les plus actifs,
	- de l'historique des 10 derniers links passés en émission,
	- des blocages RRFSentinel en cours,
	- des données techniques de l'ESP32,
	- des données techniques de l'activité du Soleil (propagation, SFI, SN, A Index, K Index, etc.),
	- des données techniques du positionnement de l'ISS (altitude, longitude, latitude, visibilité, etc.),
- ventilation par nombre de TX dans la journée, sous forme d'histogramme,
- scanner permanent de suivi d'activité sur les autres salons,
- mise en veille automatique de l'écran, en cas d'absence d'activité pendant un temps donné.

## Concernant le pilotage de son link (ou hotspot)
Voici la liste des fonctionnalités actuelles :

- QSY vers un salon,
- activation et désactivation du RRFRaptor,
- bascule en mode _Perroquet_,
- bascule en mode _Follow_,
- appel de fonctions spéciales _librement paramétrables_ par le Sysop
	- reboot du Spotnik,
	- adresse IP du Spotnik,
	- scan rapide du Raptor,
	- libre.

> __Remarque 1__
> 
> Concernant le modèle Core2, tactile, il est désormais possible d'afficher une télécommande DTMF 100% tactile. Il suffit de cliquer sur le haut de l'écran pour l'afficher.

> __Remarque 2__
> 
> Si le mode _Follow_ est activé, le RRFRemote affichera les informations du salon sur lequel se trouve votre link et il le suivra au grès de ses QSY. Il ne sera pas possible de consulter les informations d'un autre salon, à l'aide des boutons gauche et droit, à moins évidement de désactiver le mode _Follow_.

> __Remarque 3__
> 
> Concernant l'appel de fonctions spéciales paramétrables par le Sysop, elles sont au nombre de 4. C'est plutôt une fonctionnalité pour les _power user_ ayant un minimum de compétence en scripting. Par défaut, et à titre d'exemple, 3 fonctions sont déjà définies : reboot et adresse IP du Spotnik ainsi que le scan rapide du Raptor. Libre à vous de les modifier ou de définir celle qui reste libre. Il vous suffit :
> 
> - d'adapter le code du script `RRFRemote.js`. Ces appels de fonctions correspondent aux traitements des appels des commandes `cmd` 2000, 2001, 2002 et 2003 du script `RRFRemote.js`.
> - de renommer ces fonctions dans le fichier `RRFRemote.h` (voir la définition du tableau de pointeurs `const char *sysop[]`).

## Serveur Web intégré 

Cette fonctionnalité permet de visualiser votre RRFRemote depuis un simple navigateur. Il est même possible de le piloter par ce biais, dans la mesure ou les boutons sont cliquables. Afin d'afficher votre RRFRemote dans votre navigateur, il suffit d'aller sur `http://adresse_ip_de_votre_rrfremote/`. Pour rappel, l'adresse IP que récupère votre RRFRemote s'affiche sur l'écran d'accueil, à l'allumage. Il est également indiquée dans données techniques de l'ESP32 (affichage alterné complémentaire).

> Attention : c'est lent ! Et il n'y a pas de rafraîchissement automatique. Il faut cliquer sur le fond de l'image de l'écran pour faire une nouvelle > capture. Et sinon, comme dit, les boutons sont fonctionnels.

Mais pourquoi avoir développé une telle fonctionnalité ? Pour 2 raisons : 

- premièrement, c'était amusant à développer,
- deuxièmement, ca permet de prendre de belle capture d'écran du RRFRemote, la preuve ;)

![Capture](https://github.com/armel/RRFRemote/blob/main/img/expert.png)
![Capture](https://github.com/armel/RRFRemote/blob/main/img/basic.png)


## Contrôle de l'état des connexions réseau

Un petit cercle est affiché en haut, à droite de l'écran de votre RRFRemote. Il permet, à tout moment, de contrôler l'état des connexions réseau. En l'occurence : 

- si ce cercle est plein (rempli), tout est nominal : connexion Wifi et connexion avec le Spotnik. 
- si ce cercle est vide : il y a une perte *temporaire* de connexion avec le Spotnik. Pas de panique, ça arrive. Mais sauf problème grave, ça ne dure jamais. En complément, le message défilant affichera *Perte de connexion Spotnik*.
- si ce cercle est invisible : il n'y a carrément plus de connexion Wifi. Le RRFRemote va automatiquement tenter de se reconnecter au point d'accès en cours d'utilisation. En complément, le message défilant affichera *Perte de connexion Wifi* (en mode très saccadé, c'est normal, la reconnexion Wifi prenant l'essentiel du CPU...).

> Si la perte de connexion Wifi est normale (par exemple, vous n'êtes plus chez vous), le plus simple est d'éteindre le RRFRemote. Ce dernier tentera de se reconnecter aux différents points d'accès définis dans votre fichier `settings.h` (et pourra donc se reconnecter à votre partage de connexion Smartphone, si vous l'avez prévu).

## En complément 
Il est possible de :

- changer le mode d'affichage : EXPERT ou BASIC (pour un affichage avec une taille de police plus importante),
- changer les couleurs du thème,
- régler la luminosité de l'écran,
- activer une alerte sonore TOT (_alerte visuelle via la LED sur le CORE2_),
- passer d'une configuration à une autre.

# Installation

> En complément des instructions probablement trop succintes que je décris ci-dessous, je ne peux que vous conseiller de lire l'excellent [message](https://groups.io/g/RRF/message/2444) publié par F6FDY Eddie, sur le groupe RRF. Il donnera de précieux conseils aux OMs qui n'ont pas l'habitude de déployer des environnements de développement. 

## Pré-ambule

Le plus simple est d'installer [PlateformIO for VSCode](https://platformio.org/install/ide?install=vscode) sur votre PC (Linux ou Windows) ou sur votre Mac (Intel ou M1). C'est un environnement de développement multiplateforme et multilangage performant, en plus d'être agréable à utiliser.

> Quelques OMs ont eu des difficultés à installer PlateformIO sous Windows. Le message d'erreur le plus courant est:
> 
> ```
> The terminal process failed to launch: Path to shell executable "c:\Users\armel\RRFRemote\platformio.exe" does not exist.
> Terminal will be reused by tasks, presse any key to close it.
> ```
> 
> Afin de contourner ce problème, rechercher les __Alias d'exécution de l'appli__ et décocher les occurences __Installeur d'applications python.exe__ et __python3.exe__. Puis fermer et relancer VSCode.
> 
> ![Windows](https://github.com/armel/RRFRemote/blob/main/img/Windows.jpeg)
>
> 
> Un grand merci à F5LGW Stéphane pour l'information et VA2NRJ Robin pour la capture d'écran.


Ensuite, toujours sur votre PC ou Mac, cloner le projet RRFRemote via la commande `git` :

`git clone https://github.com/armel/RRFRemote.git`

Vous pouvez aussi récuppérer une archive zip, avec l'ensemble du projet, si vous n'avez pas `git`.

## Coté Spotnik : mise en place du script de contrôle

Copier le script `RRFRemote.js` situé dans le sous répertoire `src/Spotnik` sur votre Spotnik (par exemple, dans le répertoire `/root`). 

Lancer le script via la commande : 

`nohup node /root/RRFRemote.js &`

Pour qu'il se lance automatiquement en cas de reboot, ajouter la ligne suivante dans votre `/etc/rc.local` :

``
nohup node /root/RRFRemote.js & 
``

## Coté PC : paramétrage, compilation et flashage du M5Stack

Ouvrez le projet RRFRemote avec PlateformIO for VSCode.

### Paramétrage

#### Fichier `src/RRFRemote.h`

Petit confidence : je hais la gestion des décalages horaires, des alternances hiver/été, etc. Néanmoins, si vous êtes hors de France, il est désormais possible de configurer votre _timezone_ afin qu'elle soit prise en compte lors de l'affichage de l'heure. Ligne 74, la variable `ntpTimeZone` est configurée, par défaut, pour la France. Mais j'ai indiqué d'autres configurations possibles pour d'autres régions du monde, régulièrement actives sur le RRF. Il vous suffit de décommenter uniquement la _timezone_ qui correspond à votre région. Si elle n'est pas présente, consultez cette [liste](https://github.com/blindsidenetworks/bigbluebutton-1/blob/master/bbb-voice-conference/config/freeswitch/conf/autoload_configs/timezones.conf.xml) plus détaillée. En dernier recours, me contacter si besoin d'aide ou si vous habitez dans une région exotique...

#### Fichier `src/settings.h`

Editer le fichier `src/settings.h` afin de renseigner vos paramétrages via le tableau `config`, à savoir :

* votre SSID Wifi,
* votre mot de passe Wifi,
* votre latitude (format décimale, par exemple 48.8482855),
* votre longitude (format décimale, par exemple 2.2708201),
* votre indicatif,
* et pour finir, l'url d'accès au script de contrôle que vous avez installé précédement (par défaut, c'est `http://adresse_ip_de_votre_spotnik:3000/`).

> __Remarque 1__
> 
> Si vous n'avez pas de Spotnik, vous pouvez tout de même utiliser le RRFRemote. Il suffit juste de ne pas indiquer d'url. Une telle configuration pourrait ressembler à ceci :

```
const char *config[] = {
  "Livebox", "mdp_Livebox", "48.8482855", "2.2708201", "F4HWN H", "",
};
```

> __Remarque 2__
> 
> À noter que, depuis la version 2.0.0, il est possible de gérer autant de configurations que vous le souhaitez ! Cela vous permet, par exemple, de préconfigurer vos paramétrages pour une utilisation locale sur le Wifi de votre QRA et une configuration mobile sur le Wifi de votre Smartphone. Une telle configuration pourrait ressembler à ceci :
> 

```
const char *config[] = {
  "Livebox", "mdp_Livebox", "48.8482855", "2.2708201", "F1ZPX H", "http://192.168.1.10:3000/",
  "iPhone8", "mdp_iphone", "48.8482855", "2.2708201", "F1ZPX M", "http://82.167.56.2:3000/" 
};
```

> Notez au passage l'utilisation d'une adresse IP publique pour la configuration mobile sur le Wifi du Smartphone...

Toujours dans le fichier `src/settings.h`, si vous disposez d'un module aditionnel M5GO (BASIC, GREY ou FIRE) ou M5GO2 (CORE2 ou AWS), vous pouvez ajouter des indicatifs à surveiller via le tableau `qrz`. Ce tableau est constitué d'une liste d'indicatifs et d'une couleur associée au format RGB. Si l'un de ces indicatifs passe en émission, les barres de leds présentent sur les modules M5GO et M5GO2 s'allumeront (avec la couleur associée). Cela permet d'avoir des alertes luminueuses.

> Vous pouvez vous contenter d'indiquer seulement une partie d'un indicatif. Par exemple ```"F", "0xff0000"```, idéalement positionné en fin de liste, permettra d'allumer les leds en rouge, à chaque passage en émission d'une station donc l'indicatif commence par F (et non listé plus haut dans la liste).

#### Fichier `platformio.ini`

Le RRFRemote peut fonctionner sur un écran externe, via une connexion HDMI. Cela permet de profiter d'un affichage plus grand et donc, pour certains, plus confortable. 

Afin de disposer d'une connectique HDMI, il existe actuellement une alternative (donc 2 solutions) :

- soit disposer d'un ATOM Display,
- soit disposer d'un Display Module 13.2 (compatible avec les CORE et CORE2). 

##### Utilisation du Display Module 13.2 (CORE et CORE2)

Si et seulement si __vous utilisez le Display Module 13.2__, éditer le fichier `platformio.ini` et modifier éventuellement la ligne 35, afin de préciser la résolution de votre écran. Attention, le RRFRemote s'affichera toujours en 320 x 240, mais sera centré à l'écran. Par défaut, la résolution de l'écran est initialisée à 320 x 240. Mais si vous souhaitez la changer en 480 x 320, modifier la ligne 35,

```
build_flags = ${env.build_flags} -D atom=0 -D displayWidth=320 -D displayHeight=240
```

Par,

```
build_flags = ${env.build_flags} -D atom=0 -D displayWidth=480 -D displayHeight=320
```

##### Utilisation de l'ATOM Display (ATOM uniquement)

Si et seulement si __vous utilisez l'ATOM Display__, éditer le fichier `platformio.ini` et modifier la ligne 12,

```
default_envs = m5stack
```

Par,

```
default_envs = atom
```

Cela revient à changer la plate-forme cible.

En complément, vous pouvez également préciser la résolution de votre écran. Attention, le RRFRemote s'affichera toujours en 320 x 240, mais sera centré à l'écran. Par défaut, la résolution de l'écran est initialisée à 320 x 240. Mais si vous souhaitez la changer en 480 x 320, modifier la ligne 43,

```
build_flags = ${env.build_flags} -D atom=1 -D displayWidth=320 -D displayHeight=240
```

Par,

```
build_flags = ${env.build_flags} -D atom=1 -D displayWidth=480 -D displayHeight=320
```

### Compilation et flashage

Compiler et flasher le projet sur votre M5Stack. C'est terminé.

## Mise à jour

>Je ne peux que vous conseiller de garder une copie de votre ancien fichier `settings.h`. Ca vous fera probablement gagner du temps. Attention cependant, le fichier `settings.h` a changé depuis la version 2.0.0. 

Si vous souhaitez mettre à jour la version de votre RRFRemote, vous pouvez :

* soit télécharger l'archive zip, si vous n'avez pas `git`,
* soit effacer le répertoire existant, utiliser les commandes `git` et cloner à nouveau le projet, comme lors de la première installation,
* soit se positionner dans le répertoire existant, utiliser les commandes `git` et mettre à jour le code à l'aide des commandes suivantes :

```
git reset --hard
git pull
```
Evidement, vous devrez recompiler et uploader le projet sur votre M5Stack.

## Utilisation du Bin Loader (_power user only..._)

Evolution récente de mes développements, il est désormais possible de stocker plusieurs applications dans la mémoire SPI Flash de votre M5Stack ou sur une carte SD. Au démarrage, une procédure est prévue pour charger une application en particulier. 

> Attention, ce Bin Loader ne fonctionne pas avec l'ATOM Display.

### Préparation

Je vais détailler ici la procédure pour déployer l'application RRFRemote et DXTracker sur un même M5Stack.

#### Etape 1 - Compilation

Commencez par compiler vos applications, comme vous aviez l'habitude de le faire. Rien ne change ici. Par exemple, commencez par compiler l'application RRFRemote. Puis faites de même avec l'application DXTracker. 

> À noter que les applications ICSMeter et ICMultimeter disposent aussi du Bin Loader.
> 
#### Etape 2 - Collecte des fichiers binaires

Ca y est, vous avez compiler l'application RRFRemote et DXTracker ? C'est parfait.

Chaque compilation a produit un binaire. C'est ce binaire qui est envoyé / flashé sur votre M5Stack, via la connexion USB.

Placez vous à la racine du dossier RRFRemote, qui contient l'ensemble du projet. Et allez dans le répertoire `.pio/build/m5stack`.

Vous y trouverez un fichier `firmware.bin`. 

Vous avez maintenant 2 solutions. 

A vous de choisir celle que vous préférez : 

- stockage sur carte SD : simple, offre une grosse capacité de stockage, mais nécessite d'avoir une carte disponible,
- stockage sur la mémoire SPI Flash : plus compliqué, capacité de stockage réduite, mais aucun prérequis.

#### Etape 2.1 - Stockage sur la carte SD (simple)

Formater une carte SD au format FAT32.

Copier le fichier `firmware.bin` de l'application RRFRemote et placer le à la racine de la carte SD. Et profitez en pour le renommer en l'appelant, par exemple, `RRFRemote.bin`.

Faites de même avec l'application DXTracker. Copier le fichier `firmware.bin` de l'application DXTracker et placer le à la racine de la carte SD.  Et profitez en pour le renommer en l'appelant, par exemple, `DXTracker.bin`.

Vous devez désormais avoir 2 fichiers binaires à la racine de votre carte SD : `RRFRemote.bin` et `DXTracker.bin`.

#### Etape 2.2 - Stockage sur la mémoire SPI Flash (plus difficile)

Copier le dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote. Et profitez en pour le renommer en l'appelant, par exemple, `RRFRemote.bin`.

> Si le dossier `data` n'existe pas, créé le.

Faites de même avec l'application DXTracker. Placez vous à la racine du dossier DXTracker, qui contient l'ensemble du projet. Et allez dans le répertoire `.pio/build/m5stack`.

Vous y trouverez également un fichier `firmware.bin`. Copier le, lui aussi, dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote. Et profitez en pour le renommer en l'appelant, par exemple, `DXTracker.bin`.

> **Hyper important, l'idée est bien de copier ces 2 binaires dans le même répertoire `data`** (situé à la racine du dossier RRFRemote).

A ce stade, vous devez donc avoir 2 fichiers binaires clairement identifiés : `RRFRemote.bin` et `DXTracker.bin` dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote.

#### Etape 3 - Copie dans la mémoire Flash du M5Stack

Passons à l'étape probablement la plus compliquée. Ouvrez le projet RRFRemote depuis Visual Studio Code, comme vous le feriez pour le compiler. 

![Capture](https://github.com/armel/RRFRemote/blob/main/img/flash_1.png)

Etape 1, cliquez sur l'icône Platformio (l'icone avec une tête de fourmi...). Etape 2, déroulez la section `m5stack`.

![Capture](https://github.com/armel/RRFRemote/blob/main/img/flash_2.png)

Enfin, étape 3, allez dans la sous section `Platform`. Et cliquez sur `Upload Filesystem Image`.

Patientez. Le contenu du répertoire `data` va être écrit dans la mémoire Flash de votre M5Stack. Ca y est ? Vous y êtes !!!

### Utilisation

Démarrez votre M5Stack. Vous devriez voir un écran noir, suivi de l'affichage de 1, 2 puis 3 petits points, en haut de l'écran. C'est le fameux Bin Loader ;)

Dès l'affichage du premier petit point, vous pouvez :

- soit appuyez sur le bouton gauche ou droite, pour lancer l'application par défaut.
- soit appuyez sur le bouton central. Dans ce cas, le menu du Bin Loader s'affiche et vous propose la liste des binaires disponnibles en mémoire Flash. 

Si vous avez parfaitement suivi la procédure, vous devriez avoir le choix entre `RRFRemote.bin` et `DXTracker.bin`.

Les boutons gauche et droite, permettent de passer d'un binaire à un autre. Et le bouton central permet de valider le binaire en cours de sélection. Dans ce cas, l'application selectionnée sera chargée ;)

> Le chargement prend environ 20 secondes. C'est supportable.

### Limitation

Je pense qu'il est possible de faire cohabiter 3 ou 4 applications, dans la mémoire Flash de votre M5Stack. En l'état, ca me semble suffisant. Si besoin, j'adapterais le code pour les binaires sur la carte SD. 

# Remerciements

Un grand merci à F4ICR Pascal, F6FDY Eddie, F5LGW Stéphane, F4IGC Philippe et tous les autres, pour la confiance, les idées et l'aide qu'ils m'ont apporté depuis le début de ce projet. Avoir des OMs impliqués qui suivent de près mes développements est toujours stimulant ;)

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 

