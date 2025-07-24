let clienteWeb = null;

const clientId = 'Esp32' + Math.floor(Math.random() * 900) + 100;
clienteWeb = new Paho.MQTT.Client('broker.hivemq.com', 8884, clientId);

clienteWeb.connect({   
    useSSL: true, 
    onSuccess: function() {
        alert('A conexão com Broker foi bem sucedida')
    },
    onFailure: function() {
        alert('A conexão com Broker falhou')
    }
});

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
        
        // Criar mensagem MQTT para ligar
        const msg = new Paho.MQTT.Message("1");
        msg.destinationName = `senai928/lampada/${comodo}/ligar`;
        clienteWeb.send(msg);
    } else {
        lampada.classList.remove('ligada');
        botao.textContent = 'Ligar';
        botao.classList.remove('desligado');
        
        // Criar mensagem MQTT para desligar
        const msg = new Paho.MQTT.Message("0");
        msg.destinationName = `senai928/lampada/${comodo}/desligar`;
        clienteWeb.send(msg);
    }

    console.log(`Lâmpada da ${comodo}: ${estadoLampadas[comodo] ? 'ligada' : 'desligada'}`);
}


