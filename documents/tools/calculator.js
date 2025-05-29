// Array of construction messages
const messages = [
    "Building awesome features...",
    "Polishing the user experience...",
    "Adding the finishing touches...",
    "Testing everything twice...",
    "Making sure it's perfect..."
];

let currentMessage = 0;

// Function to update the status message
function updateMessage() {
    const statusElement = document.getElementById('status-message');
    statusElement.style.opacity = '0';
    
    setTimeout(() => {
        statusElement.textContent = messages[currentMessage];
        statusElement.style.opacity = '1';
        currentMessage = (currentMessage + 1) % messages.length;
    }, 500);
}

// Update message every 3 seconds
setInterval(updateMessage, 3000);

// Add smooth transition for status message
document.getElementById('status-message').style.transition = 'opacity 0.5s ease-in-out';

// Optional: Add current year to make it feel more dynamic
const currentYear = new Date().getFullYear();
console.log(`Construction page loaded - ${currentYear}`);