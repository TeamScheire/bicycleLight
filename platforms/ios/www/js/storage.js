var storage = {
    setItem: function (referenceName, object) {
        try {
            var objectAsString = JSON.stringify(object);
            window.localStorage.setItem(referenceName, objectAsString);
        } catch (error) {
            console.log(error);
        }
    },
    getItem: function (referenceName) {
        try {
            var objectAsString = window.localStorage.getItem(referenceName);
            var object = JSON.parse(objectAsString);
            return object;
        } catch (error) {
            console.log(error);
        }
    },
    removeItem: function (referenceName) {
        try {
            window.localStorage.removeItem(referenceName);
            return true;
        } catch (error) {
            console.log(error);
        }
    }
}