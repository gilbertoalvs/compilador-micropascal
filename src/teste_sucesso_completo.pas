program Geometria;
var
    raio, area : real;
    opcao : char;
    contador : integer;
begin
    opcao := 'a';
    raio := 5.5;
    contador := 0;
    
    if opcao = 'a' then
        area := 3.14 * (raio * raio);
    else
        area := 0.0;
        
    while contador < 3 do
    begin
        write(area);
        contador := contador + 1;
    end;
end.