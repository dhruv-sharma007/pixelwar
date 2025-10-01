import type { Context } from "hono";
import User from "../models/user.model";
import { nanoid } from "nanoid";

interface IBody {
    name: string;
}

export const signIgUser = async (c: Context) => {
    try {
        const body = await c.req.json();
        const { name } = body as IBody;
        console.log(name);
        if (!name || typeof name !== "string") {
            c.status(400);
            return c.json({ message: "Name is required" });
        }

        const existingUser = await User.findOne({ name });

        if (existingUser) {
            c.status(200);
            return c.json({
                message: "User sign-in successful",
                name: existingUser.name,
                deviceId: existingUser.deviceId,
                userId: existingUser.userId,
            });
        }

        const userId = nanoid(5);

        const newUser = await User.create({
            name,
            authType: "guest",
            deviceId: "2342sfafsac@!DASD",
            userId,
        });

        if (!newUser) {
            c.status(500);
            return c.json({ message: "Something went wrong" });
        }

        c.status(201);
        return c.json({
            message: "User created and signed in successfully",
            name: newUser.name,
            deviceId: newUser.deviceId,
            userId: newUser.userId,
        });
    } catch (err) {
        c.status(500);
        return c.json({ message: "Server error", error: String(err) });
    }
};
