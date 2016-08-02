using UnityEngine;
using System.Collections;

public class FiringScript : MonoBehaviour
{
    public GameObject projectile;
    public GameObject FiringPoint;

    public float speed = 100;

	// Use this for initialization
	void Start ()
    {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
	    
	}

    public void Fire()
    {
        //GameObject bullet = Instantiate(projectile, FiringPoint.transform.position, FiringPoint.transform.rotation) as GameObject;
        //Rigidbody rb = bullet.GetComponent<Rigidbody>();
        //rb.AddRelativeForce(Vector3.forward * speed);


        GetComponent<PhotonView>().RPC("FireOnAll", PhotonTargets.All, new object[] { FiringPoint.transform.position, FiringPoint.transform.rotation });
    }

    [PunRPC]
    public void FireOnMaster(Vector3 pos, Quaternion rot)
    {
        GameObject nb = PhotonNetwork.InstantiateSceneObject("projectile", pos, rot, 0, null) as GameObject;
        Rigidbody rb = nb.GetComponent<Rigidbody>();
        rb.AddRelativeForce(Vector3.forward * speed);

    }

    [PunRPC]
    public void FireOnAll(Vector3 pos, Quaternion rot)
    {
        GameObject bullet = Instantiate(projectile, pos, rot) as GameObject;
        Rigidbody rb = bullet.GetComponent<Rigidbody>();
        rb.AddRelativeForce(Vector3.forward * speed);
    }
}
