
# AUTOBAUD Commands
## AT Command

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>

## Error Command

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    ERROR&ltCR&gt&ltLF&gt
</font>

## RSSI Value in ASU

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+CSQ<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    +CSQ: 31,99&ltCR&gt&ltLF&gt<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>

## Status of Network Registration

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+CREG?<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    +CREG: 0,1&ltCR&gt&ltLF&gt<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>

## Selected Network Operator

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+COPS?<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    +COPS: 0,0,"Idea"&ltCR&gt&ltLF&gt<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>

## List of Network Operators

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+COPN<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    +COPN: "001010","Test PA128-PA4"&ltCR&gt&ltLF&gt<br>
    +COPN: "00101","Test PA128-PA4"&ltCR&gt&ltLF&gt<br>
    +COPN: "20201","GR COSMOTE"&ltCR&gt&ltLF&gt<br>
    +COPN: "20205","vodafone GR"&ltCR&gt&ltLF&gt<br>
    +COPN: "310160","T-Mobile"&ltCR&gt&ltLF&gt<br>
    +COPN: "31016","T-Mobile"&ltCR&gt&ltLF&gt<br>
    +COPN: "310170","AT&T"&ltCR&gt&ltLF&gt<br>
    +COPN: "356110","C&W"&ltCR&gt&ltLF&gt<br>
    +COPN: "90115","OnAir"&ltCR&gt&ltLF&gt<br>
    +COPN: "90117","Navitas1"&ltCR&gt&ltLF&gt<br>
    +COPN: "90118","Maritime Wireless"&ltCR&gt&ltLF&gt<br>
    ...<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>

## IMEI

<font face="Cascadia Code" size="4">
    Transmit:<br>
    AT+CGSN<br><br>
    Receive:<br>
    &ltCR&gt&ltLF&gt<br>
    359357080000000&ltCR&gt&ltLF&gt<br>
    &ltCR&gt&ltLF&gt<br>
    OK&ltCR&gt&ltLF&gt
</font>