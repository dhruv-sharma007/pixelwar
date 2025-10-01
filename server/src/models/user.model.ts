import mongoose from "mongoose";

const UserSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true
    },
    userId: {
        type: String,
        required: true
    },
    deviceId: {
        type: String
    },
    authType: {
        type: String
    }
})

const User = mongoose.model("User", UserSchema);
export default User;