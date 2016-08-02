using UnityEngine;
using System.Collections;
using Photon;

public class SmoothPosUpdates : Photon.MonoBehaviour
{
    private Vector3 correctPlayerPos;
    private Quaternion correctPlayerRot;

	void Update ()
    {
	    if(!photonView.isMine)//this updates the objects in the scene that arnt mine
        {
            transform.position = Vector3.Lerp(transform.position, this.correctPlayerPos, Time.deltaTime * 5);
            transform.rotation = Quaternion.Lerp(transform.rotation, this.correctPlayerRot, Time.deltaTime * 5);
        }
	}

    void OnPhotonSerializeView(PhotonStream stream, PhotonMessageInfo info)
    {
        if(stream.isWriting)
        {
            //we own this player send the others our data
            stream.SendNext(transform.position);
            stream.SendNext(transform.rotation);
            //Debug.Log("sending pos + rot : " + transform.position.ToString());
        }
        else
        {
            //network player, recieve the data
            this.correctPlayerPos = (Vector3)stream.ReceiveNext();
            this.correctPlayerRot = (Quaternion)stream.ReceiveNext();
        }
    }
}
