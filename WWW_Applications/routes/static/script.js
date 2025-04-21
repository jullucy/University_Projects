window.onload = function() {
    const image = document.getElementById("background-preview");
    const pointsDataElement = document.getElementById("points-data");
    const points = JSON.parse(pointsDataElement.innerText);

    // Draw existing points on the background
    points.forEach(point => {
    addMarker(point.x, point.y, point.id);
    });

    // Event listener for the image to add new points
    image.addEventListener('click', function (event) {
        const rect = image.getBoundingClientRect();

        // Ensure the click is within the image bounds
        if (event.clientX < rect.left || event.clientX > rect.right ||
            event.clientY < rect.top || event.clientY > rect.bottom) {
            return; // Ignore clicks outside the image
        }

        // Calculate relative click position
        const clickX = event.clientX - rect.left;
        const clickY = event.clientY - rect.top;

        const relativeX = Math.round((clickX / rect.width) * 1000);
        const relativeY = Math.round((clickY / rect.height) * 1000);

        addMarker(relativeX, relativeY);
        savePoint(relativeX, relativeY);
    });

    // Function to add a marker to the image
    function addMarker(x, y) {
        const rect = image.getBoundingClientRect();
        const marker = document.createElement("div");
        marker.classList.add("point-marker");

        const percentX = (x / 1000) * rect.width;
        const percentY = (y / 1000) * rect.height;

        marker.style.position = "absolute";
        marker.style.left = percentX + "px";
        marker.style.top = percentY + "px";

        document.getElementById("preview-container").appendChild(marker); // Append marker relative to the image's parent
    }


    // Function to save point to the server or database
    function savePoint(pointX, pointY) {
        const csrfToken = document.querySelector('[name=csrf-token]').content;

        const dataToSend = {
            x: pointX,
            y: pointY
        };

        fetch(window.location.href, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRFToken': csrfToken
            },
            body: JSON.stringify(dataToSend)
        })
            .then(response => response.json())
            .then(data => {

                console.log('Saved successfully!', data);
            })
            .catch(error => {
                console.error('Error:', error);
            });
    }

};
