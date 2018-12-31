# PW-projekcik
<h1>Warcaby angielskie</h1>

Wymagają przy każdym nowym uruchomieniu serwera zmiany klucza (zmienna key, zdefiniowana w obu plikach źródłowych) lub (lepsza wersja) po przerwaniu działania serwera nalezy usunąć wykorzystywaną kolejkę komunikatów poprzez wpisanie komendy "ipcrm -q <id kolejki, wyświetla się podczas działania serwera>" w konsoli.

PROSZĘ O TESTOWANIE i informowanie o zauważonych błędach w działaniu!!! 

Z góry zastrzegam że program nie jest "idiotoodporny" i może słabo znosić błędny input (zwłaszcza zbyt długi).

<h3>GRA</h3>
Plansza jest przechowywana w tablicy dwuwymiarowej 8x9 pól; rząd numer 0 jest pusty, żeby zachować klasyczną numerację pól od 1.<br/>
Pierwszy indeks oznacza kolumnę, drugi natomiast wiersz. <br/>
Kolumny są oznaczane enumem zawierającym kolejne, wielkie litery alfabetu.<br/>
<b>Rzędy numerowane są od 1!</b><br/>
Póki co gra jest osobnym programem, ponieważ tak jest łatwiej ją testować.

<br/><p>Okazuje się, że nie jest łatwo aż tak nagiąć zasady warcabów, żeby w żaden sposób nie zmuszać gracza do bicia. Chodzi mi tu o serię bić - po zbiciu pionka przeciwnika, 
jeśli gracz może zbić tym samym pionkiem kolejny, to musi to zrobić. Także zaimplementowałem, nazwijmy to, "słaby" przymus bicia. Polega to na tym, że nie przeglądam wszystkich 
pionków gracza poszukując tych, które mogą bić, a jedynie sprawdzam, czy podniesiony pionek ma możliwiość bicia. <sub>PJT</sub></p>

<h3>TODO:</h3>
<ul>
<li>Dodać warunek zwycięstwa</li>
<li>Podpiąć warcaby pod serwer</li>
<li>Wprowadzić komunikację gry z klientem</li>
<li>Poprawić możliwe błędy w ruchu pionków</li>
<li>Poprawić obsługę błędów</li>
<li>Dokończyć dokumentację</li>
<li>???</li>
<li>Profit :)</li>
</ul>