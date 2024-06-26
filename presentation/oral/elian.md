# Présentation oral d'Elian

## Slide 11 :
> Système de collision cercle/rectangle

Pour que le joueur ne quitte pas l'environnement de jeu, nous avons implanté un système de collision cercle/rectangle, où le joueur est représenté par le cercle et les murs par les rectangles.
Dans un premier temps, nous cherchons le point le plus proche du mur par rapport au joueur à l'aide d'une fonction de clampage, qui ramène une valeur à un intervalle.

Dans un second temps, nous regardons si la distance entre le joueur et ce point est plus petite que le rayon du cercle.
Si c'est le cas, le joueur est en collision avec le mur et nous le repositionnons en dehors du mur.
Sinon, il 

## Slide 24 :
> Les portails

Les portails sont des outils mis à disposition du personnage pour lui permettre de créer un passage et de passer d'un point A à un point B plus rapidement.
On les utilise à l'aide du clique gauche et droit et se positionnent sur les murs.

Pour detecter l'utilisation des portails, on réutilise le même système de collision qu'utilisé pour les murs.

Pour téléporter le joueur, on calcule un nouvel angle, ce qui revient à reporter l'angle du joueur en le tournant d'un certain nombre de quart de tours pour garder un mouvement fluide et naturel.
On doit aussi calculer sa nouvelle position.
On la calcule en prenant soit le symétrique axiale par rapport à l'abscisse ou à l'ordonnée, soit une symétrie centrale par rapport au milieu du portail, puis en effectuant une translation en suivant un vecteur allant du centre du premier portail au centre du second.

## Slide 26 :
> Conclusion

Pour conclure, nous avons réussi à mettre en place la majorité des éléments que nous voulions implanter.
Nous avons crée un environnement de jeu, où nous avons une grille de jeu, un système de collision, un rendu texturé et des portails qui fonctionnent.
Nous avons aussi réussi à mettre en place un raycaster en utilisant l'algorithme DDA.

Cependant, notre projet peut encore être complété/amélioré.
Nous pourions ajouter des scènes pour améliorer l'ergonomie.
L'amélioration principale que nous pourions faire serait d'ajouter la vue à travers le portail.
Nous pourions aussi ajouter des niveaux supplémentaires et ajouter un marquage de fin de niveau.

Merci de votre attention.
