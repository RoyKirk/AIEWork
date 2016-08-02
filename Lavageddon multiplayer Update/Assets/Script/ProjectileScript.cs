using UnityEngine;
using System.Collections;

public class ProjectileScript : MonoBehaviour
{
    public float dmg = 1;
    public GameObject explosion;
    public GameObject explosionEffect;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
	    if(transform.position.y < -1)
        {
            PhotonNetwork.Destroy(gameObject);
        }
	}

    void OnTriggerEnter(Collider c)//call an RPC to master client to deal with destroying things ect. the bullet will need to destroy itself on each client, but collision is on master only?
    {
        if(c.tag == "Block")
        {
            c.GetComponent<BlockDamage>().Damage(dmg);
            Instantiate(explosion, transform.position, Quaternion.identity);
            Instantiate(explosionEffect, transform.position, Quaternion.identity);
            PhotonNetwork.Destroy(gameObject);
        }
    }

    [PunRPC]
    void CollisionOnMaster()
    {
        //not sure if i need to do this here, as most of the collision happens in block function
    }
}
