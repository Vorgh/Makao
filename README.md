# Server-Client-Makaó

###Leírás

Ez a project egy egyszerű szerver-kliens alapú makaó játék. Eddig még csak localhost-on volt tesztelve.  
Egyenlőre terminálban működik szöveges alappal, de tervezett egy grafikus felület a klienseknek.  
A szerver C-ben, a kliens JAVA-ban íródott.  

###Játék indítása

**Szerver:**  
gcc makao_server.c -o makao_server  
./makao_server 2  
  Argumentumként kell megadni a játékosok számát (2-4)

**Kliens:**  
javac makao_client.java  
java makao client 2222  
Jelenleg a 2222-es porton figyel a szerver, ez a makao_server.c #define részében átírható.

###Játékmenet
Minden kártyának egy egyedi 2 karakteres kódja van, az első a színt, a második a számot/figurát jelenti.  
Pl.:A piros lapok:
* P7 -> Piros 7
* P8 -> Piros 8
* P9 -> Piros 9
* P0 -> Piros 10
* PA -> Piros alsó
* PF -> Piros felső
* PK -> Piros király
* PS -> Piros ász  
  
Minden játékos üzenetet kap, amikor az ő köre van. Ilyenkor egy lap sorszámát megadva le lehet rakni.  
A **kérőlapok** a következők működnek:  
Alsóval számot, felsővel színt lehet kérni. Például ha az 1-es sorszámú lapunk egy felső, akkor az "1 PX" paranccsal pirosat kértünk. Alsónál ez "1 X8"-ra módosul, amivel 8-ast kértünk.

A játékszabályok a hivatalos játékszabályok, kivéve, hogy kérőlapokra nem lehet kontrázni, és egyszerre csak egy lapot lehet lerakni.  
Ha valaki nyer, a szerver és a kliens folyamatok automatikusan befejeződnek.
