
// Estado das lâmpadas
const estadoLampadas = {
    sala: false,
    cozinha: false,
    quarto: false,
    banheiro: false
};

function toggleLampada(comodo) {
    const lampada = document.getElementById(`lampada-${comodo}`);
    const botao = document.getElementById(`btn-${comodo}`);

    estadoLampadas[comodo] = !estadoLampadas[comodo];

    if (estadoLampadas[comodo]) {
        lampada.classList.add('ligada');
        botao.textContent = 'Desligar';
        botao.classList.add('desligado');
    } else {
        lampada.classList.remove('ligada');
        botao.textContent = 'Ligar';
        botao.classList.remove('desligado');
    }

    // Aqui você pode adicionar código para comunicar com o ESP32
    console.log(`Lâmpada da ${comodo}: ${estadoLampadas[comodo] ? 'ligada' : 'desligada'}`);
}
