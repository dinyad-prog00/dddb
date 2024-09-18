import { Socket } from "net";
import Collection from "./collection";
import { ObjectType, FilterOptions, Options } from "../types";

class Model {
    collection: Collection;

    constructor(client: Socket, dbName: string, collectionName: string) {
        this.collection = new Collection(client, dbName, collectionName)
    }

    async getAll(options?: Options) {
        return await this.collection.exec({ verb: "GET", single: false, options })
    }

    async find(filter?: FilterOptions,options?: Options) {
        if (filter) {
            return await this.collection.exec({ verb: "GET", single: false, where: Object.keys(filter).length === 0 ? undefined : filter ,options})
        }
        return await this.collection.exec({ verb: "GET", single: false, options })
    }

    async findOne(filter: FilterOptions,options?: Options) {
        return await this.collection.exec({ verb: "GET", single: true, where: filter,options })
    }

    async findById(_id: string,options?: Options) {
        return await this.collection.exec({ verb: "GET", single: true, where: { _id } ,options})
    }

    async create(data: ObjectType) {
        return await this.collection.exec({ verb: "POST", data: data, single: true })
    }

    async update(filter: FilterOptions, data: ObjectType) {
        return await this.collection.exec({ verb: "PUT", where: filter, data: data, single: false })
    }

    async updateOne(filter: FilterOptions, data: ObjectType) {
        return await this.collection.exec({ verb: "PUT", where: filter, data: data, single: true })
    }

    async updateById(_id: string, data: ObjectType) {
        return await this.collection.exec({ verb: "PUT", where: { _id }, data: data, single: true })
    }

    async delete(filter: FilterOptions) {
        return await this.collection.exec({ verb: "DELETE", where: filter, single: false })
    }

    async deleteOne(filter: FilterOptions) {
        return await this.collection.exec({ verb: "DELETE", where: filter, single: true })
    }

    async deleteById(_id: string) {
        return await this.collection.exec({ verb: "DELETE", where: { _id }, single: true })
    }

}

export default Model