using UnityEngine;
using System.Collections;

using Photon;

public class RandomMatchmaker : PunBehaviour
{
    GameObject spawnpos;
    Vector3 pos;

    // Use this for initialization
    void Start()
    {
        PhotonNetwork.ConnectUsingSettings("0.1");
        PhotonNetwork.logLevel = PhotonLogLevel.Full;
        pos = GetComponent<Transform>().position;
    }

    void OnGUI()
    {
        GUILayout.Label(PhotonNetwork.connectionStateDetailed.ToString());
    }

    public override void OnJoinedLobby()
    {
        //base.OnJoinedLobby();
        PhotonNetwork.JoinRandomRoom();
    }

    void OnPhotonRandomJoinFailed()
    {
        Debug.Log("Can't join random room!");
        PhotonNetwork.CreateRoom(null);
    }

    //public GameObject spawnBlock;

    public override void OnJoinedRoom()
    {
        int playerlist = PhotonNetwork.playerList.Length;
        spawnpos = GameObject.Find("Spawn (" + playerlist + ")");
        transform.position = spawnpos.transform.position;
        transform.rotation = spawnpos.transform.rotation;
        Vector3 blockOffset = transform.position + transform.forward * 10;
        GetComponent<PhotonView>().RPC("CreateBlockOnMaster", PhotonTargets.MasterClient, new object[] { blockOffset, true });
    }

}
