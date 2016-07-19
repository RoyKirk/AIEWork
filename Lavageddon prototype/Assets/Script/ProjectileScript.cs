using UnityEngine;
using System.Collections;

public class ProjectileScript : MonoBehaviour
{
    public float dmg = 1;
    public GameObject explosion;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
	    if(transform.position.y < -5)
        {
            Destroy(this.gameObject);
        }
	}

    void OnTriggerEnter(Collider c)
    {
        if(c.tag == "Block")
        {
            c.GetComponent<BlockDamage>().Damage(dmg);
            Instantiate(explosion, transform.position, Quaternion.identity);
            Destroy(this.gameObject);
        }
    }
}
