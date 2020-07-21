export const createMutex = () => {
    let token = true;
    return (f: any) => {
        if (token) {
            token = false;
            try {
                f();
            } finally {
                token = true;
            }
        }
    };
};