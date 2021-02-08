# RRFRemote
Le projet RRFRemote permet de suivre l'activité du RRF et de piloter son point d'accès ou hotpsot. Il peut fonctionner en charge (via un connecteur USB) ou sur batterie. 

# Architecture technique en bref

Basé sur un ESP-32, dual core, pouvant être cadencé jusqu'à 240 MHz, le RRFRemote fonctionne parfaitement à 80 MHz. Il dispose de 16 MB de mémoire flash. Comme tous les ESP, le Wifi est évidement intégré. L'écran 2 pouces TFT couleur, basé sur le chipset ILI9342C, affiche une résolution de 320 x 240 pixels. Il est hyper lumineux. La batterie intégrée fait 110 mAh. Il est possible d'ajouter une batterie supplémentaire (de 700 ou 800mAh) si besoin. En terme de dimensions et de masse, c'est très compact : 54 x 54 x 18mm pour 47,2g. Se trimbale dans la poche sans problème ;)

# Fonctionnalités

Le développement du RRFRemote est toujours en cours.

## Concernant le suivi de l'activité du RRF
Voici la liste des fonctionnalités actuelles :

- suivi (quasi) temps réel de l'activité des salons du RRF,
- affichage de l'indicatif du link en cours d'émission et de son temps d'émission,
- affichage alterné :
	- du nombre de links connectés,
	- du nombre de links actifs,
	- du nombre de passages en émission depuis le début de la journée,
	- du temps d'émission cumulé depuis le début de la journée,
	- de l'heure,
	
- affichage des links entrants et sortants (via scrolling horizontal),
- affichage alterné complémentaire :
	- du top 10 des links les plus actifs,
	- de l'historiques des 10 derniers links passés en émission,
	- des blocages RRFSentinel en cours,
	- des données techniques de l'ESP32,
	- des données techniques de l'activité du Soleil (propagation, SFI, SN, AIndex, KIndex, etc.)
- ventilation par nombre de TX dans la journée sous forme d'histogramme,
- scanner de suivi d'activité sur les autres salons.

## Concernant le pilotage de son link ou hotspot
Voici la liste des fonctionnalités actuelles :

- QSY vers un salon
- Activation et désactivation du RRFRaptor
- Bascule en mode Perroquet

## En complément 
Il est possible :

- de changer les couleurs du théme
- régler la luminosité de l'écran
- allumer ou éteindre le RRFRemote

#That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 

