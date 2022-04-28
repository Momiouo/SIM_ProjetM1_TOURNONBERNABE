# Projet de Synthèse d'images : Création d'un terrain en c++ OpenGL 🖼️
***
Groupe : TOURNON Pierre - BERNABE Aurélien - M1 INFO - 2021/2022

## Screenshot
***
![Image text](screenshot.png)

## Installation 
***
```
$ git clone https://github.com/Momiouo/SIM_ProjetM1.git
$ cd /SIM_PROJETM1_TOURNONBERNABE
$ qmake && make
$ ./terrain
```

## Utilisation
***
### Camera 
***
#### Touche clavier ⌨️
flèche du haut ⬆️ = déplacement en haut</br>
flèche du bas ⬇️ = déplacement en bas</br>
flèche de droite ➡️ = déplacement à droite</br>
flèche de gauche ⬅️ = déplacement à gauche</br>
Touche Z = Zoom </br>
Touche S = Dézoom

#### Souris 🖱️
Drag vers le bas  = rotation vers le haut</br>
Drag vers la haut = rotation vers le bas</br>
Drag vers la droite = rotation vers la gauche</br>
Drag vers la gauche = rotation vers la droite</br>
Clique bouton milieu = Zoom ou Dézoom 

### Animation 🎥
***
L'animation de l'eau se fait automatiquement, à chaque fois que la méthode drawscene est appelée.

## Notre organisation 📆
***
Nous avons développé le projet à deux, nous avons codé en même temps en utilisant le plugin liveshare sur visual studio code afin de pouvoir développer 
en temps réel. Pour développer les fonctionnalités nous avons utilisé git ce qui nous as permis de créer des branches sur lesquelles on pouvait développer 
des fonctionnalités telles que l'application des textures, l'apport d'une nouvelle caméra, le shadow mapping.

## Description de l'état de notre projet ⚒️
***
### Effectué
Finalement nous avons pu génerer le terrain que l'on souhaité avec l'utilisation du bruit de perlin, 
nous avons aussi appliqué les textures souhaitées tellesque la neige, la roche, l'herbe et l'eau. 

### Reste à faire 
Nous avons aussi pris du temps à développer d'autres fonctionnalité qui n'ont malheuresement pas abouti. Nous avons essayé de créer une nouvelle caméra
en mode fps pour se déplacer en toute facilité dans le terrain comme une personne mais c'était une charge de travail un peu trop grosse et nous
avons donc pas réussi à l'implémenter en totalité.  
Nous avons aussi essayé de faire un effet brouillard ainsi qu'un shadow mapping mais nous avons pas réussi à finir ces fonctionnalitées.
Nous avons laissé ces fonctionnalitées en cours de développement dans la branche "dev_branch".

## Ressources utilisées 📚
***
Une liste des ressources utilisées durant le projet:
* [PerlinNoise-ShadowMapping-FogEffect](http://maverick.inria.fr/~Romain.Vergne/blog/teaching/IS-M1-INFO/)
* [Camera](https://learnopengl.com/Getting-started/Camera)
