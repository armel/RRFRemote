# RRFRemote
Le projet RRFRemote permet de suivre l'activité du RRF et de piloter son point d'accès ou hotpsot. Il peut fonctionner en charge (via un connecteur USB) ou sur batterie. 

![RRFRemote](https://github.com/armel/RRFRemote/blob/main/img/RRFRemote.jpeg)

# Architecture technique en bref

[M5Stack](https://m5stack.com/) est basé sur un ESP-32, dual core, pouvant être cadencé jusqu'à 240 MHz.  Néanmoins, le RRFRemote fonctionne parfaitement à 80 MHz. M5Stack dispose de 16 MB de mémoire flash. Comme tous les ESP, le Wifi est évidement intégré. L'écran 2 pouces TFT couleur, basé sur le chipset ILI9342C, affiche une résolution confortable de 320 x 240 pixels. Il est hyper lumineux. La batterie intégrée fait 110 mAh. Il est possible d'ajouter une batterie supplémentaire (de 700 ou 800mAh) si besoin. 

En terme de dimensions et de masse, c'est très compact : 54 x 54 x 18mm pour 47,2g. Se trimbale dans la poche sans problème ;) 

Coté QSJ, compter autour de 25€.

Le développement actuel prend en charge le [M5Stack Basic](https://m5stack.com/collections/m5-core/products/basic-core-iot-development-kit) et le [M5Stack Grey](https://m5stack.com/collections/m5-core/products/grey-development-core). Je n'ai pas testé le M5Stack Fire, mais ca devrait fonctionner.

Attention, le développement actuel ne fonctionne pas (encore) sur les M5Stack Core2. Je ne recommmande pas ce modèle, trop jeune, et encore instable. Mais ce n'est qu'une question de temps et de maturité. Il apportera des évolutions intéressantes avec, en particulier, un écran tactile !

# Fonctionnalités

Le développement du RRFRemote est toujours en cours.

## Concernant le suivi de l'activité du RRF
Voici la liste des fonctionnalités actuelles :

- suivi _quasi_ temps réel de l'activité des salons du RRF,
- affichage de l'indicatif du link en cours d'émission et de son temps d'émission,
- affichage alterné :
	- du nombre de links connectés,
	- du nombre de links actifs,
	- du nombre de passages en émission depuis le début de la journée,
	- du temps d'émission cumulé depuis le début de la journée,
	- de l'heure,
	
- affichage des links entrants et sortants, via scrolling horizontal,
- affichage alterné complémentaire :
	- du top 10 des links les plus actifs,
	- de l'historique des 10 derniers links passés en émission,
	- des blocages RRFSentinel en cours,
	- des données techniques de l'ESP32,
	- des données techniques de l'activité du Soleil (propagation, SFI, SN, A Index, K Index, etc.)
- ventilation par nombre de TX dans la journée, sous forme d'histogramme,
- scanner permant de suivi d'activité sur les autres salons.

## Concernant le pilotage de son link (ou hotspot)
Voici la liste des fonctionnalités actuelles :

- QSY vers un salon,
- Activation et désactivation du RRFRaptor,
- Bascule en mode _Perroquet_,
- Bascule en mode _Follow_.

> Si le mode _Follow_ est activé, le RRFRemote affichera les informations du salon sur lequel se trouve votre link et il le suivra au grès de ses QSY. Il ne sera pas possible de consulter les informations d'un autre salon, à l'aide des boutons gauche et droit, à moins évidement de désactiver le mode _Follow_.

## En complément 
Il est possible de :

- changer les couleurs du thème,
- régler la luminosité de l'écran,
- allumer ou éteindre le RRFRemote.

# Installation

## Pré-ambule

Le plus simple est d'installer [PlateformIO for VSCode](https://platformio.org/install/ide?install=vscode) sur votre PC. C'est environnement de développement multiplateforme et multilangage, en plus d'être agréable à utiliser.

Ensuite, toujours sur votre PC, cloner le projet RRFRemote via la commande :

`https://github.com/armel/RRFRemote.git`

## Coté Spotnik : mise en place du script de contrôle

Copier le script `RRFRemote.js` situé dans le sous répertoire `src/Spotnik` sur votre Spotnik (par exemple, dans le répertoire `/root`). 

Lancer le script via la commande : 

`nohup node /root/RRFRemote.js &`

Pour qu'il se lance automatiquement en cas de reboot, ajouter la ligne suivante dans votre `/etc/rc.local` :

``
nohup node /root/RRFRemote.js & 
``

## Coté PC : compilation et flashage du M5Stack

Ouvrez le projet RRFRemote avec PlateformIO for VSCode.

Editer le fichier `src/settings.h` afin de renseigner vos paramétrages Wifi, votre indicatif et l'url  d'accès au script de contrôle installé précédement. Par défaut, c'est : 

`http://adresse_ip_de_votre_spotnik:3000/`

Compiler et uploader le projet sur votre M5Stack. C'est terminé.

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 

