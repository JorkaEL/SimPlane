
Cet exemple est un loader minimal de fichier au format FBX permettant
de lire un objet et ses animations. Cet exemple est une variation de
l'exemple ViewScene :

a/la gestion des differentes caméras et des lumières n'est pas pris en
compte;

b/un chargeur de fichier image de differents format a été créé
(glimage.c, glimage.h) generant des id de textures

Cet exemple comporte également un fichier main.cxx qui montre comment
charger un modèle et jouer l'une de ces animations.

Dans cet exemple figure egalement deux modeles FBX :
a/Models/ferrari_f40/Ferrari_F40.FBX (un modele fixe avec plusieurs textures)
b/Models/fish/fish.FBX (un modèle animé).

Un Makefile est egalement joint et permet une compilation sur systeme MacOsX et Linux
Deux variables sont à modifier en fonction de l'emplacement ou le sdk a été installé :

##### Mettre le chemin vers les include et les libs du sdk
INCDIR      = ../../include
LIBDIR      = ../../lib/clang/release/
##### Fin des modifs a effectuer


Vincent Boyer - Novembre 2014.
