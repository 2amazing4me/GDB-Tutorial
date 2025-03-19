# Tutorial GDB

## Prefata

### Ce este GDB?

GDB este ori cel mai bun prieten ori cel mai mare dusman al vostru in facultate.
Scopul tutorialului e sa ne fie prieten ca altfel e grav...

Il folosim pentru debugging, si il vom folosi destul (source: trust me bro). 
Chit ca lucram in C, C++, (maybe) Rust si chiar si Assembly (unde o sa folositi
probabil pwndbg, un plugin pentru GDB, more on this la IOCLA (PCLP2)).

### De ce GDB?

Pentru ca vrem sa ne simtim superiori, normal.

All jokes aside tho, in majoritatea cazurilor poate fi muuuuuuuult mai useful
decat printf-urile. Putem avea control granular asupra programului, si sa vedem
efectiv fiecare pas pe care il face programul. Folosind GDB, putem "opri" timpul
si sa vedem oricand, ce se intampla in programul nostru. De asemenea, o sa vedeti
ca exista tot felul de trucuri si tech-uri super useful pe care le puteti folosi.

In plus, asa cum probabil ati mai auzit "daca inveti C, poti invata orice limbaj de 
programare", acelasi lucru se aplica si GDB-ului. Daca inveti sa folosesti GDB-ul,
o sa poti folosi intuitiv orice alt debugger.

## Cum folosim GDB?

### Inainte de debug

Pe langa faptul ca trebuie sa avem GDB instalat, mai e un pas super important pe
care trebuie sa-l facem inainte sa intram in GDB: trebuie sa compilam programul
cu flag-ul `-g`.

```bash
gcc -o <nume_executabil> <nume_program.c> -g
```

Ce face flag-ul? Adauga informatii in executabil, informatii precum:
- numele fisierului sursa
- numele functiilor
- numele variabilelor
- linia de cod corespunzatoare fiecarei instructiuni

De ce trebuie sa adaugam flag-ul? Desi nu este **necesar**, fara flag nu putem
preciza unde vrem sa ne oprim in cod, nu putem vedea la ce linie ne aflam, nu 
putem accesa o variabila arbitrara, etc. Pe scurt, totul e ascuns.

(Nota: Ghidra, un decompiler, este un exemplu perfect pentru vizualizarea 
efectului flag-ului `-g`. In lipsa flag-ului, vom vedea doar adrese de memorie 
si nume generice. Cu flag-ul, vom vedea codul ***aproape*** la fel cum l-am scris.)

### GDB Basics

#### Pornim GDB

Am ajuns si aici, intr-un final. Sa incepem prin pornirea programului:

```bash
gdb <nume_executabil>
```

Foarte straight-forward. Dupa ce am dat comanda, o sa vedem ceva de genul:

```bash
GNU gdb (Ubuntu 9.2-0ubuntu1~20.04) 9.2
...
--Type <RET> for more, q to quit, c to continue without paging--
```

Dam un Enter si o sa mai vedem:

```bash
...
Reading symbols from a.out...
```

Daca apare si:

```bash
(No debugging symbols found in a.out)
```

Atunci ar fi cazul sa reverificam capitolul [Inainte de debug](#inainte-de-debug).

Altfel, daca nu apare linia de mai sus, am intrat cu succes in executabil si putem
incepe debug-ul.

#### Comenzi de baza

- `break` sau `b` - seteaza un breakpoint
    - `b <nume_functie>` - seteaza un breakpoint la inceputul functiei
    - `b <linie>` - seteaza un breakpoint la linia specificata
        - Daca programul are mai multe fisiere sursa, **TREBUIE** sa specificam si
        numele fisierului `b <nume_fisier>:<linie>`
- `run` sau `r` - porneste executabilul
    - Daca programul are argumente, le putem specifica aici `r <arg1> <arg2> ...`
    - Daca programul are input, putem redirectiona un fisier de input `r < <nume_fisier>`
- `next` sau `n` - executa urmatoarea linie de cod
- `continue` sau `c` - continua executia pana la urmatorul breakpoint
- `step` sau `s` - intra in apelul de functie de la linia curenta
- `finish` sau `fin`- iese din functia curenta
- `print` sau `p` - afiseaza valoarea unei variabile
    - `p <variabila>` - afiseaza valoarea variabilei
    - `p &<variabila>` - afiseaza adresa variabilei
    - `p <expresie>` - afiseaza valoarea expresiei (merg inclusiv apeluri de functie)
- `quit` sau `q` - iese din GDB

Evident, sunt mult mai multe comenzi, dar acestea sunt cele pe care le vom folosi
cel mai des. Puteti gasi liste complete pe internet.

#### Bine de stiut

* Daca breakpoint-ul se afla dupa o citire, vom avea o linie goala in terminal.
Asemenea unui program normal, scriem ce avem de dat ca date de intrare urmat de enter pana cand sunt citite toate datele.

* Dupa fiecare comanda care avanseaza in program vom intalni un prompt precum:

    ```bash
    Breakpoint 1, func (num=11) at exemplu.c:5
    5               int sol = num + 1;
    ```
    
    sau

    ```bash
    6               return sol;
    ```

    Primul prompt il primim cand ajungem la un breakpoint, iar al doilea in orice alt caz.

    Informatiile afisate totusi sunt importante:
    - `Breakpoint 1` - numarul breakpoint-ului (in ordinea setata de noi)
    - `func (num=11)` - numele functiei si valorile parametrilor
    - `at exemplu.c:5` - numele fisierului sursa si linia la care ne aflam
    - `5` sau `6` - numarul liniei de cod care **urmeaza** sa fie executata
    - `int sol = num + 1;` sau `return sol;` - linia de cod care **urmeaza** sa fie executata

    Foarte important de retinut, linia de cod afisata este cea care **urmeaza** sa
    fie executata, nu cea care a fost executata.

    Asta inseamna ca in exemplul nostru, linia `int sol = num + 1;` nu a fost executata
    inca, iar utilizarea comenzii `p sol` va intoarce o valoare garbage.

* Daca vrem sa repetam ultima comanda, putem da Enter, si se va executa ultima
comanda. Util pentru comenzi precum `next` sau `continue`

### Flow-ul unui debug

O voi spune de la inceput, nu exista o cale unica de a face debugging, insa 
niste principii comune ar trebui sa fie la baza oricum.

Personal, as sugera un flow top-down. Ce inseamna asta? Inseamna ca vrem sa
pornim de la problema si sa ajungem la cauza ei, pas cu pas.

(Ca sa intelegem mai bine ce inseamna, un flow bottom-up, ar presupune rularea
de la inceputul programului pas cu pas pana la prima problema intalnita, si
rezolvarea acestora pe masura ce sunt intalnite.

Diferenta dintre aceste strategii este ca in cazul top-down, avem o tinta clara
si stim care e problema)

Hai sa lucram pe un exemplu practic:

Vom folosi exemplul din laboratorul de [liste dublu inlantuite](https://code.devmind.ro/problem/335/127/803).

Ni se cere sa implementam toate operatiile unei liste dublu inlantuite.

Dupa ce am implementat tot codul, vrem sa testam urmatoarele functionalitati:

```
create_int
add 0 2
add 1 5
add 2 10
add 3 20
print
free
```

La rulare, observam ca primim segfault. Cum procedam?

1. **Rulam programul in GDB, FARA breakpoint-uri**

    ```bash
    gdb <nume_executabil>
    run
    ```

    Vedem ca primim urmatorul output:

    ```bash
    Program received signal SIGSEGV, Segmentation fault.

    0x00005555555553bf in dll_add_nth_node (list=0x5555555596b0, n=0, new_data=0x7fffffffdd78) at dll.c:88
    88                      new_node->next->prev = new_node;
    ```

    Deja stim unde si de ce pica programul, si nu am facut nimic special.

    La linia 88 din fisierul `dll.c`, in functia `dll_add_nth_node`, avem un segfault.

    Inculpatul este `new_node->next->prev = new_node;`. Cum procedam?

2. **Incercam sa vedem valorile variabilelor la momentul segfault-ului**

    Stai asa, nu iesi inca din GDB! In multe cazuri, inca mai ai acces la 
    variabilele din program, chiar daca programul a picat.

    Hai sa vedem care e problema:

    ```bash
    (gdb) p new_node
    $1 = (dll_node_t *) 0x5555555596d0
    ```

    OK, new_node pare sa existe, hai sa continuam

    ```bash
    (gdb) p new_node->next
    $2 = (dll_node_t *) 0x0
    ```

    **ATENTIE!** new_node->next este NULL. Asta inseamna ca new_node->next->prev
    va da segfault, pentru ca incercam sa dereferentiem un pointer NULL.

    Bun, am aflat problema si cauza. Inainte sa ne aruncam in cod, hai sa vedem cum am ajuns aici.

3. **Obtinerea contextului**

    Vrem sa aflam cum s-a ajuns in situatia asta. Stim functia cu care am avut 
    probleme, asa ca punem un breakpoint la inceputul acesteia, si rulam programul 
    pana la breakpoint. (Presupunem ca restul codului e corect, unless gasim o alta 
    problema intre timp)

    ```bash
    (gdb) b dll_add_nth_node
    Breakpoint 1 at 0x555555555329: file dll.c, line 78.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    ...
    ```

    Acum ni se cere din nou inputul, si putem ori sa il dam cu totul, ori sa
    dam comenzile una cate una.

    ```bash
    create_int
    add 0 2

    Breakpoint 1, dll_add_nth_node (list=0x5555555596b0, n=0, new_data=0x7fffffffdd78) at dll.c:78
    78              dll_node_t *new_node = (dll_node_t *)malloc(sizeof(dll_node_t));
    ```

    Bun, acum am ajuns la breakpoint si vom da cateva next-uri sa vedem unde 
    ajungem

    ```bash
    (gdb) n
    ...
    (gdb) n
    85              if (n == 0 || list->head == NULL) {
    (gdb) n
    86                  new_node->next = list->head;
    ```

    Super, am intrat pe cazul in care ori nu exista elemente in lista ori 
    inseram la inceput, hai sa vedem mai departe

    ```bash
    (gdb) n
    87                  list->head = new_node;
    (gdb) n
    88                  new_node->next->prev = new_node;
    ```

    Hopa! Deci noi tocmai am setat `new_node->next = list->head` (adica NULL, 
    fiind prima inserare in lista) si acum incercam sa dereferentiem 
    `new_node->next->prev`.

    Acum solutia depinde de cum vrem sa ne scriem codul, dar acum stim clar
    care e problema si cum am ajuns aici.

## Cand folosim GDB?

Avem multe motive pentru care sa folosim GDB, dar probabil vom folosi pentru unul
din urmatoarele motive:

- Programul nu face ce vrem noi
- Programul da segfault
- Programul nu se termina
- Programul nu face ce vrem noi (jur ca nu am Alzheimer, doar ca se intampla foooaaarte des)

## Cuvinte finale

Ghidul asta e doar un inceput, sunt multe subtilitati, comenzi, trucuri, 
strategii, etc. pe care le puteti invata. Nu va stresati sa le invatati pe toate
din start, se prind pe parcurs. 

Din experienta mea, daca va apucati de acum (devreme) sa invatati GDB, o sa va
vina natural deja din anul 2-3, si e un understatement sa spun ca m-a ajutat mult
la teme, laburi sau intelegerea efectiva a codului, algoritmilor sau protocoalelor.

La fel ca orice good habit, better start early. Spor la debug, si sper sa va fie
de folos tutorialul!