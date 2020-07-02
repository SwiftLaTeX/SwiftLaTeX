export class EventReporter {
    static reportEvent = (cat: string, action: string, label: string | object = '') => {
        const handle = (window as any).meow;
        if (handle) {
            if (typeof label === 'object') {
                label = JSON.stringify(label);
            }
            handle('send', 'event', cat, action, label);
        }
    };

    static reportPageView = () => {
        const handle = (window as any).meow;
        if (handle) {
            const page = document.location.pathname;
            handle('set', 'page', page);
            handle('send', 'pageview');
        }
    }

    static reportTiming = (cat: string, action: string, value: number) => {
        const handle = (window as any).meow;
        if (handle) {
            handle('send', 'timing', cat, action, value);
        }
    }
}