# Projet de Synthèse d'images : Création d'un terrain en c++ OpenGL 🖼️
***
TOURNON Pierre - BERNABE Aurélien - M1 INFO - 2021/2022

## Screenshot
***
![Image text](screenshot.png)

## Installation 
***
```
$ git clone https://github.com/Momiouo/SIM_ProjetM1.git
$ cd /SIM_PROJETM1
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
L'animation de l'eau se fait automatiquement, à chaque fois que drawscene est appelée.

## Description de l'état de notre projet ⚒️
***
### Effectué
* Application de textures</br>
* Utilisation du bruit de perlin</br>
* Creation d'une caméra

### Reste à faire 
* Post Processing (Fog)</br>
* Une meilleure camera car limité lorsque l'on souhaite se retourner (ne fonctionne pas)</br>
* shadow maping

## Ressources utilisées 📚
***
Une liste des ressources utilisées durant le projet:
* [PerlinNoise](http://maverick.inria.fr/~Romain.Vergne/blog/teaching/IS-M1-INFO/)
* [Camera](https://learnopengl.com/Getting-started/Camera)
