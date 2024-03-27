# Présentation oral

#### Slide 0

Bonjour à tous et à toutes.
Aujourd'hui nous allons vous faire la présentation de notre projet semestriel intitulé Portal 0.0 réalisé par Elian BADSTÜBDER, Matthieu BIDAULT et moi même Vital FOCHEUX.

#### Slide 1

Avant tous nous allons vous présenter le plan de cette présentation. 
Tout d'abord nous commencerons par expliquer ce qu'est Portal 0.0 au niveau de la partie du système de jeu et la partie technique
Ensuite nous détaillerons les algorithmes et les techniques utilisées lors de notre projet, que cela soit pour la construction des murs, les collisions, ainsi que les rendus
Après cela, nous expliquerons les différentes implémentation utilisées pour les rendus, ainsi que les portails.
Enfin nous conclurons sur un bilan technique de notre projet.

#### Slide 2

Portal 0.0 est un jeu qui reprend les principes techniques de plusieurs jeux vidéos connus.
Tout d'abord sur le système de jeu, où le principe est de résoudre des énigmes à l'aide de portails. Avec une téléportation du joueur lorsqu'on passe à travers.
C'est le principe du jeu Portal (2007)

#### Slide 3

Sur la technique graphique, Portal 0.0 utilise une méthode de raycasting qui permet un rendu 3D dans un environnement en 2D. Cette technique va nous permettre d'avoir un rendu en 2.5D qui a été fortement popularisé dans le début des années 90.
C'est le principe du jeu Wolfenstein3D (1992)

#### Slide 4

Lors de ce projet, nous avons utilisé plusieurs technologies. D'abord celles qui nous ont été imposé c'est-à-dire le langage C++ avec lequel nous avons développé en adéquation de la bibliothèque Gamedev Framework qui permet la création de jeux vidéos en 2D.
Nous avons aussi utilisé le site github pour nous faciliter le partage du code

#### Slide 5

Maintenant nous allons détailler les aspects techniques de projet, en commençant d'abord par la construction des murs

#### Slide 6

Avant tout d'expliquer comment sont construit les murs, il faut comprendre à partir de quoi ils sont créer.
Les murs sont créer à partir d'une carte faite en png.
Pour recupérer les coordonnées de chacune des cellules des murs nous effectuons un parcours en profondeur.
Les coordonnées des cellules correspondent au coin supérieur gauche de chaque cellule
Une cellule correspond à un bloc unitaire de la carte

#### Slide 7

Pour construire les murs nous n'auront besoin que des sommets utiles. Nous allons effectuer une boucle sur les coordonnées des cellules et afin de savoir si notre sommet est utile ou non nous allons calculer la parité du nombre de cellules adjacentes c'est-à-dire plus exactement sa propre cellule, celle juste au dessus, celle à sa gauche et celle en haut à gauche. Si la parité est pair alors le sommet n'est pas utile, à contrario si elle est impair alors c'est un sommet utile. Dans cette exemple nous pouvons voir que tous les sommets entourés seront des sommets utiles car ils auront tous une parité impair.

#### Slide 8

Afin de faciliter la construction des murs nous allons trier les sommets utiles.
Voici un pseudo-code qui explique ce que nous faisons.
d correspond à un tableau de caractère pour chaque direction, dir et dir2 correspondent au direction que nous prendrons de base ainsi que sa direction opposé.
Nous effectuons une boucle tant uv qui est le tableau des sommets utiles contient un sommet à la direction souhaité ou celle opposé.
Nous allons utilisé une fonction nommé canGo avec la direction en paramètre qui nous renvoie true si il existe un sommet utile dans la direction donné en paramètre ou sinon false.
Si il existe un sommet utile dans la direction de base et aucun dans la direction opposé alors on va ajouter à v qui est la tableau des sommets utiles trier le sommet dans la direction dir
Sinon on va ajouter le sommet dans la direction dir2 dans v
Puis on va échanger dir et dir2 pour partir dans la direction opposé comme celle de base