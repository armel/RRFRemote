# RRFRemote
![basic](https://img.shields.io/badge/M5Stack-BASIC-black)
![grey](https://img.shields.io/badge/M5Stack-GREY-lightgrey)
![fire](https://img.shields.io/badge/M5Stack-FIRE-red)
![core2](https://img.shields.io/badge/M5Stack-CORE2-white)
![coreS3](https://img.shields.io/badge/M5Stack-CORES3-darkgrey)
![aws](https://img.shields.io/badge/M5Stack-AWS-orange)

Le projet RRFRemote permet de suivre l'activité du RRF et de piloter son point d'accès ou hotpsot. Il peut fonctionner en charge (via un connecteur USB) ou sur batterie. 

![RRFRemote](https://github.com/armel/RRFRemote/blob/main/img/RRFRemote.jpeg)

# Préambule

Désormais, l'application RRFRemote est disponible sous forme de firmware. Il est possible de l'installer depuis l'application M5Burner développée par M5Stack. Vous n'avez donc plus besoin de déployer VSCode et Platformio, d'éditer le code source et de recompiler le code à chaque fois. 

Néanmoins, le code source de l'ancienne version est toujours disponible, même s'il n'évoluera plus. Et vous pouvez toujours consulter l'ancienne [documentation](https://github.com/armel/RRFRemote/blob/main/_README.md) associée. 

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

Le développement actuel du RRFRemote prend en charge l'ensemble des M5Stack Core (à boutons) et Core2 (tactiles). La version pour M5Stack CoreS3 est encore expérimentale.

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
	- libre,
	- libre.

> __Remarque 1__
> 
> Concernant le modèle Core2, tactile, il est désormais possible d'afficher une télécommande DTMF 100% tactile. Il suffit de cliquer sur le haut de l'écran pour l'afficher.

> __Remarque 2__
> 
> Si le mode _Follow_ est activé, le RRFRemote affichera les informations du salon sur lequel se trouve votre link et il le suivra au grès de ses QSY. Il ne sera pas possible de consulter les informations d'un autre salon, à l'aide des boutons gauche et droit, à moins évidement de désactiver le mode _Follow_.

> __Remarque 3__
> 
> Concernant l'appel de fonctions spéciales paramétrables par le Sysop, elles sont au nombre de 5. C'est plutôt une fonctionnalité pour les _power user_ ayant un minimum de compétence en scripting. Par défaut, et à titre d'exemple, 3 fonctions sont déjà définies : reboot et adresse IP du Spotnik ainsi que le scan rapide du Raptor. Libre à vous de les modifier ou de définir celle qui reste libre. Il vous suffit d'adapter le code du script `RRFRemote.js`. Ces appels de fonctions correspondent aux traitements des appels des commandes `cmd` 2000, 2001, 2002, 2003 et 2004 du script `RRFRemote.js`.

## Serveur Web intégré 

Cette fonctionnalité permet de visualiser votre RRFRemote depuis un simple navigateur. Il est même possible de le piloter par ce biais, dans la mesure ou les boutons sont cliquables. Afin d'afficher votre RRFRemote dans votre navigateur, il suffit d'aller sur `http://adresse_ip_de_votre_rrfremote/`. Pour rappel, l'adresse IP que récupère votre RRFRemote s'affiche sur l'écran d'accueil, à l'allumage. Elle est également indiquée dans données techniques de l'ESP32 (affichage alterné complémentaire) ou dans le menu settings.

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
- si ce cercle est invisible : il n'y a carrément plus de connexion Wifi. 

## En complément 
Il est possible de :

- changer le mode d'affichage : EXPERT ou BASIC (pour un affichage avec une taille de police plus importante),
- changer les couleurs du thème,
- régler la luminosité de l'écran,
- activer une alerte sonore TOT (_alerte visuelle via la LED sur le CORE2_),
- passer d'une configuration à une autre.

# Installation

## Coté Spotnik (si vous en possédez un)

Le RRFRemote n'a pas nécessairement besoin d'un Spotnik pour fonctionner. À ce titre, il peut être utilisé par un simple SWL, s'il le souhaite, afin de suivre l'activité du RRF. 

Mais si vous possédez un Hotspot ou si vous êtes Sysop d'un point d'accès, assurez vous que le script `RRFRemote.js` fonctionne. Depuis une connexion SSH, vous pouvez lancer le script via la commande : 

`nohup node /root/RRFRemote.js &`

Afin de lancer le script `RRFRemote.js` automatiquement, à chaque redémarrage, le plus simple est d'éditer le fichier `/etc/rc.local` et décommenter la ligne :

```
#nohup node /root/RRFRemote.js &
```

Cette ligne se trouve en fin de fichier `/etc/rc.local`. Il suffit d'enlever le `#`. La ligne devient alors :

```
nohup node /root/RRFRemote.js &
```

Coté Spotnik, c'est terminé.

## Coté M5Stack

### Prérequis

Vous avez besoin d'une M5Stack (avec une mémoire flash de 16 Mo), d'une carte micro SD et d'un PC sous Windows, Linux ou MacOS, avec les pilotes USB installés et l'application M5Burner version 3.0.0 (ou supérieure).

### Carte micro SD

Préparez et formatez une carte micro SD en FAT32. Cette carte micro SD sera utilisée pour stocker vos fichiers de configuration et pour stocker éventuellement d'autres firmwares.

### Installation des pilotes USB

Veuillez vous rendre sur la [page de téléchargement] (https://docs.m5stack.com/en/download) afin de télécharger le pilote USB correspondant à votre système d'exploitation et installez-le.

### Installation de l'application M5Burner

Veuillez vous rendre sur la [page de téléchargement] (https://docs.m5stack.com/en/download) afin de télécharger l'application M5Burner (version 3.0.0 ou supérieure) correspondant à votre système d'exploitation et installez-la. 

### Flashage du M5Stack

Connectez votre M5Stack à votre PC.

Lancez l'application M5Burner et sélectionnez le firmware RRFRemote correspondant à votre modèle de M5Stack (Core, Core2, etc.). Vous pouvez-vous aider du moteur de recherche intégré à M5Burner pour trouver le firmware RRFRemote.

> Attention, il existe une version pour le M5Stack Core (Basic, Grey, Fire, etc.) avec boutons et une version pour le M5Stack Core2 (Core2, AWS, etc.) avec écran tactile. La version pour M5Stack CoreS3 est encore expérimentale.

Cliquez sur le bouton bleu `Download`. Cliquez ensuite sur le bouton rouge `Burn` en prenant soin de bien selectionner le port USB sur lesquel est branché votre M5Stack. 

Votre M5Stack sera flashé avec la dernière version du firmware RRFRemote.

### Fichiers de configuration

Il vous reste à créer votre ou vos fichiers de configurations. Dans ce [répertoire](https://github.com/armel/RRFRemote/tree/main/ini), vous trouverez 3 exemples de fichiers de configuration à adapter par vos soins.

| Fichier           | Description             | 
| -------------- | -------------------- |
| `Tiny.ini` | Fichier [simple](https://github.com/armel/RRFRemote/blob/main/ini/Tiny.ini) avec uniquement la section requise [wifi]|
| `Huge.ini` | Fichier [complet](https://github.com/armel/RRFRemote/blob/main/ini/Huge.ini) avec toutes les sections|
| `SWL.ini` | Fichier pour les [SWL](https://github.com/armel/RRFRemote/blob/main/ini/SWL.ini) n'ayant pas de point d'accès|

Le plus judicieux est de partir du fichier [complet](https://github.com/armel/RRFRemote/blob/main/ini/Huge.ini). Editez-le à l'aide d'un simple éditeur de texte (Notepad, Notepad++, SublimeText, etc.) et renseignez les informations qui vous concerne. Les différentes sections sont décrites un peu plus loin.

Copiez ce fichier de configuration, à la racine de votre carte Micro SD (formatée en FAT32). Libre à vous de le renommer. Vous pouvez aussi en sauvegarder plusieurs, si vous le désirez. Par exemple, une version pour gérer votre Hotspot depuis votre box Internet de votre QRA, une autre depuis votre partage de connexion Smartphone, etc.   

#### Description des sections

Voici une description des sections. Seule la section [wifi] est requise, pour des raisons évidentes.

| Section        | Requis | Valeur    |	 Description  | Défaut | 
| -------------- | ------ | --------- | ------------- | ------ |
| [wifi]			|        |           |               |        |
|		          |✔| `wifi_ssid`    | votre SSID Wifi|       |
|		          |✔| `wifi_password` | votre Password Wifi|  |
| [spotnik]		||               |                    | |
|		          || `spotnik_name` | le QRZ de votre point d'accès | `M5Stack`|
|		          || `spotnik_url`  | l'url HTTP d'accès au script RRFRemote.js | |
| [geolocation]	||               |                    | |
|		          || `latitude` | votre latitude | `48.84`|
|		          || `longitude`  | votre longitude | `2.27`|
|		          || `timezone` | votre timezone | `CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00` |

> Remarque : la timezone par défaut correspond à la France (Europe/Paris). Si vous devez adapter la timezone, consultez cette [liste](https://github.com/blindsidenetworks/bigbluebutton-1/blob/master/bbb-voice-conference/config/freeswitch/conf/autoload_configs/timezones.conf.xml) plus détaillée. En dernier recours, me contacter si besoin d'aide ou si vous habitez dans une région exotique... Voici néanmoins quelques exemples de valeurs :

| Pays           | Timezone             | 
| -------------- | -------------------- |
| Europe/Paris | CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00 |
| Europe/Brussels | CET-1CEST,M3.5.0,M10.5.0/3 |
| Europe/Sofia | EET-2EEST,M3.5.0/3,M10.5.0/4 |
| America/Montreal | EST5EDT,M3.2.0,M11.1.0 |
| America/Martinique | AST4 |
| America/Guadeloupe | AST4 |
| Pacific/Noumea | NCT-11 |

Il existe aussi les sections suivantes. Elles sont complémentaires et vous n'en aurez peut-être pas besoin.

| Section        | Requis | Valeur    |	 Description  | Défaut |
| -------------- | ------ | --------- | ------------- | ------ |
| [timeout]	||               |                    | |
|		          || `timeout_tot_rrf` | réglage du TOT sur le salon d'appel en secondes | `115`|
|		          || `timeout_tot_elsewhere`  | réglage du TOT sur les salons annexes en secondes | `285` |
|		          || `timeout_menu`  | réglage du delai de sortie automatique du menu en secondes  | `10` |
| [alert]		||               |     __maximum 64 valeurs__               | |
|		          || `alert_01` | couple indicatif / couleur RGB | | 
|		          || `...` | | |
|		          || `alert_64` | couple indicatif / couleur RGB | |
| [sysop]		||               |    __maximum 5 valeurs__                | |
|		          || `sysop_01` | couple libellé / code commande | `REBOOT, 2000` |
|		          || `sysop_02` | couple libellé / code commande | `IP, 2001` |
|		          || `sysop_03` | couple libellé / code commande | `SCAN, 2002` |
|		          || `sysop_04` | couple libellé / code commande | `LIBRE, 2003` |
|		          || `sysop_05` | couple libellé / code commande | `LIBRE, 2003` |

> Remarque : les alertes sont uniquement utiles si vous possédez un module M5GO (Core) ou M5GO2 (Core2). Voici un exemple :

```
; Alert Config
[alert]
alert_01 = F4HWN, 0xffffff
alert_02 = F5NLG, 0x00ff00
alert_03 = F1EVM, 0x00ff00
alert_04 = F4GGU, 0x00ff00
alert_05 = F1TZO, 0x00ff00
alert_06 = F4VSJ, 0x00ff00
alert_07 = F1ZPX, 0xffffff
alert_08 = F5LGW, 0x0000ff
alert_09 = F4ICR, 0x0000ff
alert_10 = FG5GP, 0x0000ff
alert_11 = FM,    0xffa000
alert_12 = FG,    0xffa000
alert_13 = F,     0x202080
alert_14 = ON,    0x800000
```

# Utilisation

Démarrez votre M5Stack. Au moment du chargement du iniLoader (gauge de progression bleu), appuyez sur le bouton du milieu. Si vous avez correctement inséré votre carte Micro SD et que votre fichier de configuration s'y trouve, il devrait être listé. Les boutons gauche et droite, permettent de passer d'un fichier de configuration, à un autre, si vous en avez plusieurs. Et le bouton central permet de valider le fichier de configuration en cours de sélection et de le chargé.

S'il ne comporte aucune erreur, l'application RRFRemote devrait démarrer.

À notez qu'une copie du dernier fichier de configuration valide est effectuée dans la mémoire de votre M5Stack. La selection d'un fichier de configuration ne sera donc plus nécessaire au prochain redémarrage. Par contre, si vous apportez des modifications à ce fichier, il faudra le recharger à nouveau !

Amusez-vous bien 🤗

# Remerciements

Un grand merci à F4ICR Pascal, F6FDY Eddie, F5LGW Stéphane, F4IGC Philippe et tous les autres, pour la confiance, les idées et l'aide qu'ils m'ont apporté depuis le début de ce projet. Avoir des OMs impliqués qui suivent de près mes développements est toujours stimulant 😌

Si vous trouvez ce projet amusant et utile, alors [offrez-moi un verre de vin](https://www.paypal.me/F4HWN) 🍷 😉 

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 

