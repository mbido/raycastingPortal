# Présentation oral de Matthieu

## Slide 11 : 
> Différentes stratégies pour un rendu 3D

Nous allons maintenant regarder les algorithmes que nous avons utilisé pour 
le rendu 3D du jeu. Pour cela, deux stratégies on été envisagées, toutes
deux utilisant un algorithme commun : le DDA.

## Slide 12 :
> DDA : l’algorithme commun

Historiquement, l'algorithme DDA (Digital Differential Analyzer) est un
algorithme de rasterization de droites, c'est à dire qu'il permet de
de passer d'une représentation mathématique d'une droite à une liste de
coordonnées de cellules, originellement des positions de pixels à allumer.

La puissance de DDA est qu'il repose sur l'itération linéaire. Il analyse
les coordonnées de cellules à ajouter à la liste à chaque fois qu'il sort/entre
dans une nouvelle cellule.

Dans le cas du rendu 3D, DDA est utilisé pour calculer la distance entre
le joueur et les murs. Grâce à l'iteration linéaire, DDA permet de calculer
l'intersection entre un rayon lancé par le joueur et un mur sans avoir à
regarder une infinité de points sur ce rayon.

## Slide 13 :
> Fonctionnement de DDA.





