const char* indexPage = R"=====(<!DOCTYPE html>
<html>
    <body>
        <title>Configuración de Polaris</title>
        <h1> Página de configuración de Polaris</h1>
        <form method="post" enctype="application/x-www-form-urlencoded" action="/postconfig/" id="config_form">
            <h2>Configuración del dispositivo</h2>
            <label>Temperatura máxima: </label>
            <input type="text" name="max_temp" id="max_temp" required><span class="error" id="error_max_temp"></span><br><br>
            <label>Temperatura mínima: </label>
            <input type="text" name="min_temp" id="min_temp" required><span class="error" id="error_min_temp"></span><br><br>
            <label>Frequencia de las medidas: </label>
            <input type="number" name="measures_freq" id="measures_freq" required><span class="error" id="error_measures_freq"></span><br><br>
            <h2>Configuracion de la red WIFI</h2>
            <label>SSID: </label>
            <input type="text" name="ssid" id="ssid" required><span class="error" id="error_ssid"></span><br><br>
            <label>Contraseña: </label>
            <input type="password" name="pass" id="pass" required><span class="error" id="error_pass"></span><br><br>
            <h2>Configuración del servidor</h2>
            <label>URL: </label>
            <input type="url" name="post_server" id="post_server" required><span class="error" id="error_post_server"></span><br><br>
            <label>API key: </label>
            <input type="text" name="api_key" id="api_key" required><span class="error" id="error_api_key"></span><br><br>
            <input type="submit" value="Save">
        </form>
        <script>
            const configForm = document.getElementById("config_form");
            const maxTemp = document.getElementById("max_temp");
            const minTemp = document.getElementById("min_temp");
            const measuresFreq = document.getElementById("measures_freq");
            const ssid = document.getElementById("ssid");
            const pass = document.getElementById("pass");
            const postServer = document.getElementById("post_server");
            const apiKey = document.getElementById("api_key");
    
            const errorMaxTemp = document.getElementById("error_max_temp");
            const errorMinTemp = document.getElementById("error_min_temp");
            const errorMeasuresFreq = document.getElementById("error_measures_freq");
            const errorSsid = document.getElementById("error_ssid");
            const errorPass = document.getElementById("error_pass");
            const errorPostServer = document.getElementById("error_post_server");
            const errorApiKey = document.getElementById("error_api_key");
            
            maxTemp.addEventListener("input", () => {
                checkMaxTemp();
            });

            minTemp.addEventListener("input", () => {
                checkMinTemp();
            });

            measuresFreq.addEventListener("input", () => {
                checkMeasuresFreq();
            });

            ssid.addEventListener("input", () => {
                checkSsid();
            });

            pass.addEventListener("input", () => {
                checkPass();
            });

            postServer.addEventListener("input", () => {
                checkPostServer();
            });

            apiKey.addEventListener("input", () => {
                checkApiKey();
            });
            
            configForm.addEventListener("submit", (event) => {
                let errors = false;
                
                if (!checkMaxTemp()) {
                    errors = true;
                }
                if (!checkMinTemp()) {
                    errors = true;
                }
                if (!checkMeasuresFreq()) {
                    errors = true;
                }
                if (!checkSsid()) {
                    errors = true;
                }
                if (!checkPass()) {
                    errors = true;
                }

                if (!checkPostServer()) {
                    errors = true;
                }

                if (!checkApiKey()) {
                    errors = true;
                }

                if (parseFloat(maxTemp.value) <= parseFloat(minTemp.value)) {
                    errorMaxTemp.textContent = "La temperatura máxima debe ser mayor que la mínima";
                    errors = true;
                }

                if (errors) {
                    event.preventDefault();
                }
            });

            function checkMaxTemp() {
                if (maxTemp.value.trim() === "") {
                    errorMaxTemp.textContent = "Campo obligatorio";
                    return false;
                } else if (!isFloat(maxTemp.value)) {
                    errorMaxTemp.textContent = "Formato incorrecto (Ej: 12, 12.5, 0.30)";
                    return false;
                } else if (parseFloat(maxTemp.value) < -50 || parseFloat(maxTemp.value) > 120) {
                    errorMaxTemp.textContent = "El valor debe estar entre -55 y 120";
                    return false;
                } else {
                    errorMaxTemp.textContent = "";
                    return true;
                }
            }

            function checkMinTemp() {
                if (minTemp.value.trim() === "") {
                    errorMinTemp.textContent = "Campo obligatorio";
                    return false;
                } else if (!isFloat(minTemp.value)) {
                    errorMinTemp.textContent = "Formato incorrecto (Ej: 12, 12.5, 0.30)";
                    return false;
                } else if (parseFloat(minTemp.value) < -50 || parseFloat(minTemp.value) > 120) {
                    errorMinTemp.textContent = "El valor debe estar entre -55 y 120";
                    return false;
                } else {
                    errorMinTemp.textContent = "";
                    return true;
                }
            }

            function checkMeasuresFreq() {
                if (measuresFreq.value.trim() === "") {
                    errorMeasuresFreq.textContent = "Campo obligatorio";
                    return false;
                } else {
                    errorMeasuresFreq.textContent = "";
                    return true;
                }
            }

            function checkSsid() {
                if (ssid.value.trim() === "") {
                    errorSsid.textContent = "Campo obligatorio";
                    return false;
                } else {
                    errorSsid.textContent = "";
                    return true;
                }
            }

            function checkPass() {
                if (pass.value.trim() === "") {
                    errorPass.textContent = "Campo obligatorio";
                    return false;
                } else {
                    errorPass.textContent = "";
                    return true;
                }
            }

            function checkPostServer() {
                if (postServer.value.trim() === "") {
                    errorPostServer.textContent = "Campo obligatorio";
                    return false;
                } else if (!isUrl(postServer.value)){
                    errorPostServer.textContent = "Formato incorrecto (Ej: https://ejemplo.com/, http://10.10.10.10:8080/)"
                } else {
                    errorPostServer.textContent = "";
                    return true;
                }
            }

            function checkApiKey() {
                if (apiKey.value.trim() === "") {
                    errorApiKey.textContent = "Campo obligatorio";
                    return false;
                } else {
                    errorApiKey.textContent = "";
                    return true;
                }
            }

            function isFloat(num) {
                const floatRegex = /^[-+]?\d+(\.\d+)?$/;
                return floatRegex.test(num);
            }

            function isUrl(url) {
                try {
                    new URL(url);
                    return true;
                } catch (error) {
                    return false;
                }
            }
        </script>
    </body>
</html>)=====";