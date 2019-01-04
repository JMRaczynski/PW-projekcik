# PW-projekcik
<h1>Warcaby angielskie</h1>

Wymagają przy każdym nowym uruchomieniu serwera zmiany klucza (zmienna key, zdefiniowana w obu plikach źródłowych) lub (lepsza wersja) po przerwaniu działania serwera nalezy usunąć wykorzystywaną kolejkę komunikatów poprzez wpisanie komendy "ipcrm -q <id kolejki, wyświetla się podczas działania serwera>" w konsoli.

PROSZĘ O TESTOWANIE i informowanie o zauważonych błędach w działaniu!!! 

Z góry zastrzegam że program nie jest "idiotoodporny" i może słabo znosić błędny input (zwłaszcza zbyt długi).

-------

UPDATE!!! Wrzuciłem do tej gałęzi wersję 1.1 która POWINNA obsługiwać współbieżne logowanie. Proszę o testowanie współbieżnego logowania.

<h3>GRA</h3>
Plansza jest przechowywana w tablicy dwuwymiarowej 8x9 pól; rząd numer 0 jest pusty, żeby zachować klasyczną numerację pól od 1.<br/>
Pierwszy indeks oznacza kolumnę, drugi natomiast wiersz. <br/>
Kolumny są oznaczane enumem zawierającym kolejne, wielkie litery alfabetu.<br/>
<b>Rzędy numerowane są od 1!</b><br/>
<s>Póki co gra jest osobnym programem, ponieważ tak jest łatwiej ją testować. <br/>
W wersji testowej pionki przesuwa się wpisując na stdin komendy w postaci "skąd[spacja]dokąd", np."A1 C3".</s>

<h3>Informacja dla kogoś, kto chciałby podjąć się podpięcia warcabów pod serwer :)</h3>
<p>Wśród plików gry znajduje się pseudokod, znajduje się tam cały kod potrzebny do uruchomienia warcabów.</p>
<p>Wśród plików jest również skrypt compilerun.sh. Jeśli macie zainstalowany tmux (jeśli nie, to najpierw 'sudo apt-get install tmux' :D), skrypt ten kompiluje wszystkie pliki, usuwa kolejkę komunikatów, a następnie uruchamia w jednym oknie 5 terminali (1 serwer i 4 klientów). Tylko (obecnie) serwer nie radzi sobie z kilkoma klientami logującymi się w tym samym czasie, więc ten skrypt nie działa poprawnie.</p>

<br/><p>Warcaby są już w <s>prawie</s> grywalnym stanie, jednak nie miałem czasu ich dobrze przetestować, także jakby się komuś nudziło, to zapraszam do testowania. Jeśli komuś udałoby się "zepsuć" grę, to byłbym wdzięczny za podesłanie screena z kilku ostatnich ruchów. :)<sub>PJT</sub></p>

<br/><p>Okazuje się, że nie jest łatwo aż tak nagiąć zasady warcabów, żeby w żaden sposób nie zmuszać gracza do bicia. Chodzi mi tu o serię bić - po zbiciu pionka przeciwnika, 
jeśli gracz może zbić tym samym pionkiem kolejny, to musi to zrobić. Także zaimplementowałem, nazwijmy to, "słaby" przymus bicia. Polega to na tym, że nie przeglądam wszystkich 
pionków gracza poszukując tych, które mogą bić, a jedynie sprawdzam, czy podniesiony pionek ma możliwiość bicia. (Nie było to jakoś specjalnie skomplikowane) <sub>PJT</sub></p>

<h3>TODO:</h3>
<ul>
  <li><s>Dodać warunek zwycięstwa</s></li>
  <li>Podpiąć warcaby pod serwer</li>
  <li>Wprowadzić komunikację gry z klientem</li>
  <li>Poprawić możliwe błędy w ruchu pionków</li>
  <li>Poprawić obsługę błędów</li>
  <li>Dokończyć dokumentację</li>
  <li>???</li>
  <li>Profit :)</li>
</ul>

<h1>Tutorial do gita</h1>
Jeśli ktoś chciałby korzystać z gita w terminalu, a nie ma pojęcia jak to zrobić, to polecam zapoznanie się <a href="https://git-scm.com/book/pl/v1/Podstawy-Gita"> z 2 rozdziałem tego poradnika</a>.
