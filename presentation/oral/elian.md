# Présentation oral d'Elian

## Slide 10 :
> Système de collision cercle/rectangle

Pour que le joueur ne quitte pas l'environnement de jeu, nous avons implanté un système de collision cercle/rectangle, où le joueur est représenté par le cercle et les murs par les rectantgles.
Dans un premier temps, nous cherchons le point le plus proche du mur par rapport au joueur à l'aide d'une fonction de clampage.

Dans un second temps, nous regardons si la distance entre le joueur est ce point est plus petite que le rayon du cercle. Si c'est le cas, le joueur est en collision avec le mur et nous le repositionons en dehors du mur.

## Slide 21 :
> Les portails

Les portails sont des outils mis à disposition du personnage pour lui permettre de créer un passage pour lui permettre de passer d'un point A à un point B plus rapidement.
On les utilise à l'aide du clique gauche et droit et se positionnent sur les murs.

Pour detecter l'utilisation des portails, on réutilise le même système de collision qu'utilisé pour les murs.
Pour téléporter le joueur, on calcule un nouvel angle, pour garder une impression de continuité, ainsi que sa nouvelle position basée sur le portail d'arrivée.

## Slide 22 :
> Conclusion

On conclue sur ce qui a été fait et ce qui pourrait être fait.
