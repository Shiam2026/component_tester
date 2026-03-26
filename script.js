let espIP = "";
let dataInterval;

function connectESP() {
  const ipInput = document.getElementById("ipAddress").value.trim();
  if (ipInput === "") {
    alert("দয়া করে IP Address দিন!");
    return;
  }

  // IP Address সেট করা হলো
  espIP = `http://${ipInput}`;
  document.getElementById("connectionStatus").innerText = "কানেক্টিং...";
  document.getElementById("connectionStatus").style.color = "#00bcd4";

  // আগের কোনো ইন্টারভ্যাল থাকলে ক্লিয়ার করা
  clearInterval(dataInterval);

  // প্রথমবার ডেটা এনে চেক করা
  fetchData();

  // এরপর প্রতি ১ সেকেন্ডে ডেটা আনা হবে
  dataInterval = setInterval(fetchData, 1000);
}

function fetchData() {
  if (!espIP) return;

  fetch(`${espIP}/data`)
    .then((response) => response.json())
    .then((data) => {
      document.getElementById("volt").innerText = data.voltage + " V";
      document.getElementById("percent").innerText = data.percent + " %";
      document.getElementById("connectionStatus").innerText = "🟢 কানেক্টেড!";
      document.getElementById("connectionStatus").style.color = "#4CAF50";
    })
    .catch((error) => {
      console.error("Error fetching data:", error);
      document.getElementById("connectionStatus").innerText =
        "🔴 কানেকশন ফেইল্ড! IP চেক করুন।";
      document.getElementById("connectionStatus").style.color = "#f44336";
    });
}

function toggleTest() {
  if (!espIP) {
    alert("আগে IP Address দিয়ে Connect করুন!");
    return;
  }

  fetch(`${espIP}/toggle`)
    .then((response) => {
      if (!response.ok) alert("কমান্ড পাঠাতে সমস্যা হয়েছে!");
    })
    .catch((error) => {
      console.error("Toggle error:", error);
      alert("কানেকশন এরর!");
    });
}
