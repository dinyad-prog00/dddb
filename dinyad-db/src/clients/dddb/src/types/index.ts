
export interface CmdPayload {
    type?: string
    cmd: string
    db?: string
}

export interface ReqPayload {
    type?: string,
    db?: string
    collection?: string
    verb: string
    single: boolean
    data?: { [key: string]: any }
    where?: { [key: string]: any }
    options?: { [key: string]: any }
}

export interface CollectionReqPayload {
    verb: string
    single: boolean
    data?: { [key: string]: any }
    where?: { [key: string]: any }
    options?: { [key: string]: any }
}


export type Payload = CmdPayload | ReqPayload

export type FilterOptions = { [key: string]: any }
export type Options = { [key: string]: any }
export type ObjectType = { [key: string]: any }