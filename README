        Acest program demonstrează comunicarea cu un server HTTP folosind mai multe metode pentru a
interacționa cu o aplicație web. Programul utilizează un set de funcții auxiliare definite în fișierele 
helpers.h, parson.h și requests.h, buffer.h.
Am facut mai multe comenzi:
register: Înregistrare cu un nou cont. Solicită introducerea unui nume de utilizator și a unei parole.
         În caz de succes, utilizatorul este înregistrat în sistem.

login: Autentificare cu un cont existent. Solicită introducerea numele de utilizator și a parolei. 
        În caz de succes, utilizatorul este autentificat și primește un cookie de sesiune.

enter_library: Acces la bibliotecă. Este necesară autentificarea în prealabil. 
                În caz de succes, utilizatorul primește un token JWT pentru accesul la bibliotecă.

get_books: Obținerea unei liste de cărți din bibliotecă. 
                Necesită acces la bibliotecă. Returnează o listă de obiecte JSON care reprezintă cărțile din bibliotecă.

add_book: Adăugarea unei noi cărți în bibliotecă. 
        Necesită acces la bibliotecă și furnizarea informațiilor despre carte (titlu, autor, gen, număr de pagini, editură). 
        În caz de succes, cartea este adăugată în bibliotecă.

get_book: Obținerea detaliilor despre o carte după ID-ul său. 
        Necesită acces la bibliotecă și furnizarea ID-ului cărții.
         Returnează detaliile cărții sau un mesaj de eroare în cazul în care ID-ul este invalid.

delete_book: Ștergerea unei cărți din bibliotecă după ID-ul său.
                 Necesită acces la bibliotecă și furnizarea ID-ului cărții. 
                În caz de succes, cartea este ștearsă din bibliotecă.

logout: Deautentificare din sistem. Necesită autentificarea în prealabil. 
        În caz de succes, utilizatorul este deautentificat și sesiunea este încheiată.


Am folosit  https://github.com/kgabis/parson : 
         JSON pentru a parsa răspunsurile primite de la server, pentru limbajul C (JSON cel dat in enunt)
         Scopul utilizării: transmiterea obiectelor JSON către server, precum și interpretarea răspunsurilor primite de la server.

