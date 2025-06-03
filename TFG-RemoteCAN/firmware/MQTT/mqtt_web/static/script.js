async function getData() {
    const res = await fetch('/data');
    const data = await res.json();
    document.getElementById('speed').textContent = `${data.speed} km/h`;
    document.getElementById('rpm').textContent = `${data.rpm} rpm`;
}

getData();
setInterval(getData, 2000);
