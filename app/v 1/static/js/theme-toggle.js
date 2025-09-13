document.addEventListener('DOMContentLoaded', function() {
    const themeToggle = document.getElementById('themeToggle');
    const body = document.body;    
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme) {
        body.classList.add(savedTheme + '-theme');
        themeToggle.checked = savedTheme === 'light';
    }
    
    themeToggle.addEventListener('change', function() {
        if (this.checked) {
            body.classList.remove('dark-theme');
            body.classList.add('light-theme');
            localStorage.setItem('theme', 'light');
        } else {
            body.classList.remove('light-theme');
            body.classList.add('dark-theme');
            localStorage.setItem('theme', 'dark');
        }
    });
    
    document.querySelectorAll('.token-value').forEach(item => {
        item.addEventListener('click', function() {
            const textToCopy = this.querySelector('code').textContent;
            navigator.clipboard.writeText(textToCopy).then(() => {
                const tooltip = this.querySelector('.copy-tooltip');
                tooltip.textContent = 'Copied!';
                setTimeout(() => {
                    tooltip.textContent = 'Click to copy';
                }, 2000);
            });
        });
    });
});