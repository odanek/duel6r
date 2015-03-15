                                  Duel 6 Reloaded
                                ===================
Popis:
 Duel 6 Reloaded je prepracovanou verzi me hry Duel 6. Hra obsahuje
 spoustu vylepseni - podpora joypadu, multiplatformni a odladeny kod,
 oprava spousty drobnych chyb (cervene vystriky vody, vlneni pod kameny,
 pozadi u informativnich zprav a dalsi).
 
 V souboru data/config.txt je nove mozne zapnout 32 bitove barvy
 (standardne nyni zapnuto), stupen antialiasingu a jazyk aplikace.
 
 Ke stazeni je windows a linuxova verze, pokud by chtel nekdo hru prelozit
 i pod Max OS X (nemel by to byt problem), necht se mi ozve na mail.
 
 Pro spusteni pod linuxem je treba mit nainstalovane knihovny SDL, 
 OpenGL a GLU.

Pozadavky:
 Ke spusteni hry je potreba 3D karta podporujici OpenGL (jakakoliv 
 novejsi by ho mela podporovat).

Kontakt:
	e-mail: ondrej.danek@gmail.com
	WWW   : http://www.ondrej-danek.net

	Na me WWW strance si muzete docist neco o mne a take si 
        stahnout dalsi moje hry.

Ovladani:

	Ovladani 1:

		Pohyb doleva    - sipka doleva
		Pohyb doprava   - sipka doprava
		Skok            - sipka nahoru
		Skrceni         - sipka dolu
		Strelba         - ctrl
		Zvednuti zbrane - shift

	Ovladani 2:

		Pohyb doleva    - A
		Pohyb doprava   - D
		Skok            - W
		Skrceni         - S
		Strelba         - Q
		Zvednuti zbrane - E

	Ovladani 3:

		Pohyb doleva    - H
		Pohyb doprava   - K
		Skok            - U
		Skrceni         - J
		Strelba         - O
		Zvednuti zbrane - L

	Ovladani 4:

		(na numericke klavesnici)
		Pohyb doleva    - 1
		Pohyb doprava   - 3
		Skok            - 5
		Skrceni         - Enter
		Strelba         - 0
		Zvednuti zbrane - Tecka

	 Joypad:
	        Pohyb           - doleva, doprava
	        Skok            - tlacitko 1
	        Strela          - tlacitko 2
	        Zvednuti zbrane - tlacitko 3
	        
	Dalsi:
	        Konec hry                  - ESC
	        Nova hra (v menu i ve hre) - F1
	        Restart urovne             - SHIFT + F1
	        Prepinani modu full/split  - F2 (ve hre, pouze pokud je hracu mene nez 5)
	        Smazani statistik          - F3 (v menu)
	        Prubezny stav bodu         - F4 (ve hre)
	        Screenshot                 - F10
	        Spusteni konzole           - ~
	        
	Konzole:
	       switch_render_mode        - zapina/vypina wireframe vykreslovaci mod
	       show_fps                  - ukazuje pocet fps v pravem hornim rohu
	       memory_used               - vypise velikost alokovane pameti (v release verzi pouze pod windows)
	       gl_info                   - informace o graficke karte
	       lang [cesta]              - zmena jazyka
	       volume [cislo]            - nastaveni hlasitosti hudby a zvuku 0 az 128
	       music [on/off]            - zapnuti/vypnuti hudby v menu
	       joy_scan                  - provede vyhledani nove pripojenych joypadu
	       skin [id] [8 * col]       - nastaveni skinu postavy, id = cislo postavy (0 - 7), barva je trojice heaxadecimalnich cisel 00-ff
	       gun [cislo] [true/false]  - povoli/zakaze zbran, cislo je 0 az 16
	       start_ammo_range [min] [max] - nastavi mnozstvi naboju na zacatku hry, vybere se nahodna hodnota z intervalu [min,max] 
	

Credits:
	Program, grafika - Ondrej Danek, Visual C++, SDL (www.libsdl.org), OpenGL
	XM soubor hrajici v menu je od Frankyho a jmenuje se Armada nemrtvych.
	Urovne - Ondrej Danek + Pavlina Dankova
	Betatesting, navrhy, urovne ... - Mira, Das, Honzulin, Triple T, Ludek a dalsi

Program je freeware, za jeho pouzivani nemusite platit, ale jakekoliv
zasahy do nej, vyuzivani jeho casti pro jine ucely nebo jeho 
prodavani je zakazano. Hru smite sirit pouze zdarma a nejlepe s mym 
souhlasem. Nerucim za zadne skody na vasem pocitaci zpusobene
timto programem.

